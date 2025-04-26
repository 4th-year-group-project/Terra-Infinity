import argparse
import json
import struct
import time
from copy import deepcopy
from http.server import BaseHTTPRequestHandler, HTTPServer
from random import randint
from time import sleep

import cv2
import numpy as np
from scipy.spatial import Voronoi

from biomes.climate_map import determine_biomes
from biomes.create_voronoi import get_chunk_polygons
from biomes.land_water_map import determine_landmass
from biomes.midpoint_displacement import midpoint_displacement
from generation import tools
from master_script.offload_heightmaps import terrain_voronoi
from real_rivers.river_network import RiverNetwork
from real_rivers.voronoi_map import build_world_map
from utils.point_generation import construct_points2


def fetch_superchunk_data(coords, seed, biome, parameters, river_network):
    """Fetches the heightmap data for a superchunk.

    Some terms used: 
        - Global space: The coordinate system with superchunk (0,0) at (0,0)
        - Local space: The coordinate system with the smallest x and y values of the set of polygons that overlap the target superchunk at x = 0 and y = 0.
                       Basically the set of polygons we care about translated so they sit nicely up against the x and y axis, where the coordinate
                       (smallest x, smallest y) in global space is (0, 0) in local space.

    Parameters:
    coords: Chunk coordinates
    seed: Seed value for terrain generation

    Returns:
    superchunk_heightmap: Heightmap data for the superchunk
    reconstructed_image: Image of all polygons that overlapped the superchunk
    biome_image: Image where each pixel is a number representing a biome type
    """
    start_time = time.time()
    strength_factors = [0.2, 0.3, 0.3, 0.4, 0.4]
    chunk_size = 1023

    #This gets information about all polygons that overlap the superchunk region. Outputs:
    # polygon_edges_global_space: List of edges for each polygon, in the form of (start, end) coordinates (currently not used)
    # polygon_points_global_space: List of all points for each polygon
    # shared_edges: List of edges and polygons that share each of them (currently not used)
    # polygon_ids: List of unique IDs for each polygon
    start = time.time()
    polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, polygon_centers = get_chunk_polygons(coords, seed, chunk_size, parameters)
    print(f"Get polygons : {time.time() - start}")

    start = time.time()
    #Iteratively apply midpoint displacement to the polygons, strength factors are arbitrarily chosen.
    for strength in strength_factors:
        polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids = midpoint_displacement(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, strength=strength)
    print(f"Midpoint displacement : {time.time() - start}")
    #This assigns a land or water ID to each polygon, and determines the local space coordinates for each polygon. Local space is required when we interact with a noise map when determining land/water and biomes. Outputs:
    # polygon_edges_global_space: List of edges for each polygon, in the form of (start, end) coordinates (currently not used)
    # polygon_points_local_space: List of all points for each polygon, in local space
    # land_water_ids: List of land/water IDs for each polygon (0 for water, 1 for land)
    # slice_parts: Tuple of (start_coords_x, end_coords_x, start_coords_y, end_coords_y) which tell you how "far away" the actual superchunk we want is from the origin in local space.
    # polygon_points_global_space: List of all points for each polygon, in global space
    # offsets: (smallest_x, smallest_y) in global space - needed for knowing where the biome noise map should start w.r.t global space
    start = time.time()
    polygon_edges_global_space, polygon_points_local_space, land_water_ids, slice_parts, polygon_points_global_space, offsets = determine_landmass(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, coords, seed, polygon_centers, parameters)
    print(f"Determine landmass : {time.time() - start}")

    #This determines the biome for each polygon, and generates an image where each pixel is a number representing a biome type. Outputs:
    # biomes: List of biome IDs for each polygon
    # biome_image: Image where each pixel is a number representing a biome type
    start = time.time()
    biomes, biome_image = determine_biomes(coords, polygon_edges_global_space, polygon_points_local_space, land_water_ids, offsets, seed, parameters, specified_biome=biome, chunk_size=chunk_size)
    print(f"Determine biomes : {time.time() - start}")
    #This generates the heightmap for the superchunk, and returns the heightmap, an image of all polygons that overlapped the superchunk, and the biome image.
    # superchunk_heightmap: Heightmap data for the superchunk
    # reconstructed_image: Image of all polygons that overlapped the superchunk (its big)
    # biome_image: Image where each pixel is a number representing a biome type
    # tree_placements: List of tree placements for the superchunk
    superchunk_heightmap, reconstructed_image, biome_image, tree_placements = terrain_voronoi(polygon_edges_global_space, polygon_points_local_space, slice_parts, polygon_points_global_space, biomes, coords, seed, biome_image, parameters, river_network)
    print(f"Overall Time taken: {time.time() - start_time}")
    return superchunk_heightmap, reconstructed_image, biome_image, tree_placements


def generate_heightmap(parameters, river_network):
    """Generate heightmap data based on parameters and return packed binary data."""
    seed = parameters["seed"]
    cx = parameters["cx"]
    cy = parameters["cy"]
    biome = parameters.get("biome", None)
    debug = parameters.get("debug", False)
    vx = 1026
    vy = 1026
    num_v = vx * vy
    size = 16
    biome_size = 8
    tree_size = 32

    heightmap, _, biome_data, tree_placements = fetch_superchunk_data([cx, cy], seed, biome, parameters, river_network)
    heightmap = heightmap.astype(np.uint16)  # Ensure it's uint16
    # return heightmap.tobytes()

    biome_data = biome_data.astype(np.uint8)
    tree_placements_data = np.array(tree_placements, dtype=np.float32)

    # Accounting for the x and y coordinates of the tree placements
    tree_length = len(tree_placements_data) * 2

    heightmap_bytes = heightmap.tobytes()
    biome_bytes = biome_data.tobytes()
    tree_placements_bytes = tree_placements_data.tobytes()


    header_format = "liiiiiiIiIiI"
    header = struct.pack(header_format, seed, cx, cy, num_v, vx, vy, size, len(heightmap_bytes), biome_size, len(biome_bytes), tree_size, tree_length)
    packed_data = header + heightmap_bytes + biome_bytes + tree_placements_bytes

    if debug:
        # Save debug files
        with open(f"master_script/dump/{seed}_{cx}_{cy}.bin", "wb") as f:
            f.write(packed_data)


        # Generate debug images
        header_size = struct.calcsize(header_format)
        unpacked_array = np.frombuffer(packed_data[header_size:header_size + len(heightmap_bytes)], dtype=np.uint16).reshape(1026, 1026)

        cv2.imwrite(f"master_script/imgs/{seed}_{cx}_{cy}.png", unpacked_array)

        print(f"Saved debug files for seed={seed}, cx={cx}, cy={cy}")

    return packed_data

def get_mock_data(parameters):
    mock_data_path = "data/master_script_mock_data"
    seed = parameters["seed"]
    cx = parameters["cx"]
    cy = parameters["cy"]

    sleep(randint(3, 7))

    with open(f"{mock_data_path}/{seed}_{cx}_{cy}.bin", "rb") as f:
        packed_data = f.read()

    return packed_data


class SuperchunkRequestHandler(BaseHTTPRequestHandler):

    def __init__(self, request, client_address, server):
        self.river_network = None
        self.parameters = None

        super().__init__(request, client_address, server)

    def do_GET(self):
        """Handle GET requests to the server."""
        if self.path == '/health':
            # Simple health check endpoint
            self.send_response(200)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(b'Server is running')
        else:
            # Unknown GET endpoint
            self.send_response(404)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(b'Not Found')

    def do_POST(self):
        """Handle POST requests to the server."""
        try:
            if self.path == '/superchunk':
                # Get content length from headers
                content_length = int(self.headers['Content-Length'])

                # Read the JSON data from the request body
                post_data = self.rfile.read(content_length)
                parameters = json.loads(post_data.decode('utf-8'))

                # Check for required parameters
                required_keys = {"seed", "cx", "cy"}
                missing_keys = required_keys - parameters.keys()

                if missing_keys:
                    self.send_response(400)
                    self.send_header('Content-type', 'application/json')
                    self.end_headers()
                    error_msg = json.dumps({"error": f"Missing required parameters: {', '.join(missing_keys)}"})
                    self.wfile.write(error_msg.encode())
                    return

                done = False
                if self.parameters is None:
                    self.parameters = deepcopy(parameters)
                    done = True

                if self.river_network is None or (
                    self.parameters["seed"] != parameters["seed"] or
                    self.parameters["biome_size"] != parameters["biome_size"] or
                    self.parameters["ocean_coverage"] != parameters["ocean_coverage"] or
                    self.parameters["continent_size"] != parameters["land_water_scale"] or
                    self.parameters["river_frequency"] != parameters["river_frequency"] or
                    self.parameters["river_width"] != parameters["river_width"] or
                    self.parameters["river_depth"] != parameters["river_depth"] or
                    self.parameters["river_meanderiness"] != parameters["river_meanderiness"]
                ):
                    points = construct_points2([0,0], 1023, parameters["seed"], 50, parameters["biome_size"])
                    points = np.array(points)

                    min_x, max_x = points[:, 0].min(), points[:, 0].max()
                    min_y, max_y = points[:, 1].min(), points[:, 1].max()

                    vor = Voronoi(points)
                    world_map = build_world_map(parameters["seed"], vor, min_x, max_x, min_y, max_y)
                    self.river_network = RiverNetwork(world_map)
                    self.river_network.build(parameters, 50)

                    river_width_pct = parameters["river_width"]
                    river_width = tools.map0100(river_width_pct, 0.7, 3)

                    river_meanderiness_pct = parameters["river_meanderiness"]
                    river_meanderiness = tools.map0100(river_meanderiness_pct, 0, 0.5)

                    self.river_network.spline_trees(parameters["seed"], default_meander=river_meanderiness, default_river_width=river_width)
                    self.river_network.index_splines_by_chunk()

                    # self.river_network.plot_world(points, vor)
                    # quit()


                if not done:
                    self.parameters = deepcopy(parameters)

                if parameters.get("mock_data", False):
                    packed_data = get_mock_data(parameters)
                else:
                    packed_data = generate_heightmap(parameters, self.river_network)

                self.send_response(200)
                self.send_header('Content-type', 'application/octet-stream')
                self.send_header('Content-Disposition', f'attachment; filename="heightmap_{parameters["seed"]}_{parameters["cx"]}_{parameters["cy"]}.bin"')
                self.send_header('Content-Length', str(len(packed_data)))
                self.end_headers()
                self.wfile.write(packed_data)

            else:
                self.send_response(404)
                self.send_header('Content-type', 'text/plain')
                self.end_headers()
                self.wfile.write(b'Not Found')

        except json.JSONDecodeError:
            self.send_response(400)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            error_msg = json.dumps({"error": "Invalid JSON format"})
            self.wfile.write(error_msg.encode())

        except Exception as e:
            self.send_response(500)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            error_msg = json.dumps({"error": str(e)})
            self.wfile.write(error_msg.encode())
            print(f"Error processing request: {e}")


def run_server(host="localhost", port=8000):
    """Run the superchunk server."""
    server_address = (host, port)
    httpd = HTTPServer(server_address, SuperchunkRequestHandler)
    print(f"Starting superchunk server on http://{host}:{port}")
    print(f"Health check: http://{host}:{port}/health")
    print(f"Superchunk endpoint: http://{host}:{port}/superchunk (POST)")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("Server stopped by user")
    finally:
        httpd.server_close()
        print("Server closed")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run a superchunk generation server.")
    parser.add_argument("--host", type=str, default="localhost", help="Server host address")
    parser.add_argument("--port", type=int, default=8000, help="Server port")

    args = parser.parse_args()
    run_server(args.host, args.port)
