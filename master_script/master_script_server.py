"""Example Usage:

python3 -m master_script.master_script_server --host localhost --port 8000

Then the URL to hit is: POST http://localhost:8000/superchunk

Request Format: 
- Header: Content-Type: application/json
- Body: 
{
    "mock_data": true,
    "seed": 23,
    "cx": 1,
    "cy": 0,
    "biome": null,
    "debug": true,
    "biome_size": 50,
    "ocean_coverage": 50,
    "land_water_scale": 50,
    "global_max_height": 100,
    "temperate_rainforest": {
        "max_height": 30
    },
    "boreal_forest": {
        "max_height": 40
    },
    "grassland": {
        "max_height": 40
    },
    "tundra": {
        "max_height": 50
    },
    "savanna": {
        "max_height": 25
    },
    "woodland": {
        "max_height": 40
    },
    "tropical_rainforest": {
        "max_height": 35
    },
    "temperate_seasonal_forest": {
        "max_height": 100
    },
    "subtropical_desert": {
        "max_height": 30
    }
}

Response Format:
- Header: Content-Type: application/octet-stream
- Header: Content-Disposition: attachment; filename="heightmap_23_101_100.bin"
- Header: Content-Length: Calculated based on the response data
- Body: Packet data as with original master script response
"""

import argparse
import json
import struct
import time
from http.server import BaseHTTPRequestHandler, HTTPServer
from copy import deepcopy
from random import randint
from time import sleep

import cv2
import numpy as np

from biomes.climate_map import determine_biomes
from biomes.create_voronoi import get_chunk_polygons
from biomes.land_water_map import determine_landmass
from biomes.midpoint_displacement import midpoint_displacement
from master_script.offload_heightmaps import terrain_voronoi


def fetch_superchunk_data(coords, seed, biome, parameters):
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
    polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids = get_chunk_polygons(coords, seed, chunk_size, parameters)

    #Iteratively apply midpoint displacement to the polygons, strength factors are arbitrarily chosen.
    for strength in strength_factors:
        polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids = midpoint_displacement(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, strength=strength)
    
    #This assigns a land or water ID to each polygon, and determines the local space coordinates for each polygon. Local space is required when we interact with a noise map when determining land/water and biomes. Outputs:
    # polygon_edges_global_space: List of edges for each polygon, in the form of (start, end) coordinates (currently not used)
    # polygon_points_local_space: List of all points for each polygon, in local space
    # land_water_ids: List of land/water IDs for each polygon (0 for water, 1 for land)
    # slice_parts: Tuple of (start_coords_x, end_coords_x, start_coords_y, end_coords_y) which tell you how "far away" the actual superchunk we want is from the origin in local space.
    # polygon_points_global_space: List of all points for each polygon, in global space
    # offsets: (smallest_x, smallest_y) in global space - needed for knowing where the biome noise map should start w.r.t global space
    polygon_edges_global_space, polygon_points_local_space, land_water_ids, slice_parts, polygon_points_global_space, offsets = determine_landmass(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, coords, seed, parameters)

    #This determines the biome for each polygon, and generates an image where each pixel is a number representing a biome type. Outputs:
    # biomes: List of biome IDs for each polygon
    # biome_image: Image where each pixel is a number representing a biome type
    biomes, biome_image = determine_biomes(coords, polygon_edges_global_space, polygon_points_local_space, land_water_ids, offsets, seed, specified_biome=biome, chunk_size=chunk_size)

    #This generates the heightmap for the superchunk, and returns the heightmap, an image of all polygons that overlapped the superchunk, and the biome image.
    # superchunk_heightmap: Heightmap data for the superchunk
    # reconstructed_image: Image of all polygons that overlapped the superchunk (its big)
    # biome_image: Image where each pixel is a number representing a biome type
    superchunk_heightmap, reconstructed_image, biome_image = terrain_voronoi(polygon_edges_global_space, polygon_points_local_space, slice_parts, polygon_points_global_space, biomes, coords, seed, biome_image, parameters)

    print(f"Overall Time taken: {time.time() - start_time}")
    return superchunk_heightmap, reconstructed_image, biome_image


def generate_heightmap(parameters):
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

    heightmap, _, biome_data = fetch_superchunk_data([cx, cy], seed, biome, parameters)
    heightmap = heightmap.astype(np.uint16)  # Ensure it's uint16
    biome_data = biome_data.astype(np.uint8)

    heightmap_bytes = heightmap.tobytes()
    biome_bytes = biome_data.tobytes()

    header_format = "liiiiiiIiI"
    header = struct.pack(header_format, seed, cx, cy, num_v, vx, vy, size, len(heightmap_bytes), biome_size, len(biome_bytes))
    packed_data = header + heightmap_bytes + biome_bytes
    
    if debug:
        # Save debug files
        with open(f"master_script/dump/{seed}_{cx}_{cy}.bin", "wb") as f:
            f.write(packed_data)
        with open(f"master_script/dump/{seed}_{cx}_{cy}_biome.bin", "wb") as f:
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
                
                if parameters.get("mock_data", False):
                    packed_data = get_mock_data(parameters)
                else:
                    packed_data = generate_heightmap(parameters)
                
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