"""Example Usage:

python3 -m master_script.master_script_server --host localhost --port 8000

Then the URL to hit is: POST http://localhost:8000/superchunk

Request Format: 
- Header: Content-Type: application/json
- Body: 
{
    "seed": 23,
    "cx": 101,
    "cy": 100,
    "biome": null,
    "debug": true,
    "biome_size": 50,
    "ocean_coverage": 0,
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

import cv2
import numpy as np

from biomes.climate_map import determine_biomes
from biomes.create_voronoi import get_chunk_polygons
from biomes.land_water_map import determine_landmass
from biomes.midpoint_displacement import midpoint_displacement
from master_script.offload_heightmaps import terrain_voronoi


def fetch_superchunk_data(coords, seed, biome, parameters):
    """Fetches the heightmap data for a superchunk.

    Parameters:
    coords: Chunk coordinates
    seed: Seed value for terrain generation
    biome: Specific biome (optional)
    parameters: Additional generation parameters

    Returns:
    superchunk_heightmap: Heightmap data for the superchunk
    reconstructed_image: Image of all polygons that overlapped the superchunk
    biome_image: Biome data for the superchunk
    """
    start_time = time.time()
    strength_factors = [0.2, 0.3, 0.3, 0.4, 0.4]
    chunk_size = 1023

    relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = get_chunk_polygons(coords, seed, chunk_size, parameters)
    og_polygon_points = deepcopy(relevant_polygons_points)

    for strength in strength_factors:
        relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = midpoint_displacement(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, strength=strength)
    land_polygon_edges, polygon_points, polygon_ids, slice_parts, relevant_polygons_og_coord_space, offsets = determine_landmass(relevant_polygons_edges, relevant_polygons_points, og_polygon_points, shared_edges, polygon_ids, coords, seed, parameters)
    biomes, biome_image = determine_biomes(coords, land_polygon_edges, polygon_points, polygon_ids, offsets, seed, specified_biome=biome, chunk_size=chunk_size)

    superchunk_heightmap, reconstructed_image, biome_image = terrain_voronoi(land_polygon_edges, polygon_points, slice_parts, relevant_polygons_og_coord_space, biomes, coords, seed, biome_image, parameters)
    print(f"Overall Time taken: {time.time() - start_time}")
    return superchunk_heightmap, reconstructed_image, biome_image


def generate_heightmap(parameters):
    """Generate heightmap data based on parameters and return packed binary data."""
    seed = parameters["seed"]
    cx = parameters["cx"]
    cy = parameters["cy"]
    biome = parameters.get("biome", None)
    debug = parameters.get("debug", False)
    vx = 1023
    vy = 1023
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
        with open(f"master_script/dump/{seed}_{cx-200}_{cy-200}.bin", "wb") as f:
            f.write(packed_data)
        with open(f"master_script/dump/{seed}_{cx-200}_{cy-200}_biome.bin", "wb") as f:
            f.write(packed_data)
        
        # Generate debug images
        header_size = struct.calcsize(header_format)
        unpacked_array = np.frombuffer(packed_data[header_size:header_size + len(heightmap_bytes)], dtype=np.uint16).reshape(1023, 1023)
        cv2.imwrite(f"master_script/imgs/{seed}_{cx-200}_{cy-200}.png", unpacked_array)
        
        print(f"Saved debug files for seed={seed}, cx={cx}, cy={cy}")

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