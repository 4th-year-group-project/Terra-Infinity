"""The file contains functions to parse a binary packet containing heightmap, biome data, and tree placement data. Primarily used for manual testing."""

import struct

import matplotlib.pyplot as plt
import numpy as np


def parse_packet(data):
    """Parses a binary packet containing heightmap, biome data, and tree placement data and visualizes it.

    Args:
        data: The binary data to parse.

    Returns:
        heightmap: The heightmap data as a 2D numpy array.

    """
    # Parse header fields manually
    seed = int.from_bytes(data[0:8], byteorder="little", signed=True)
    cx = int.from_bytes(data[8:12], byteorder="little", signed=True)
    cy = int.from_bytes(data[12:16], byteorder="little", signed=True)
    num_v = int.from_bytes(data[16:20], byteorder="little", signed=True)
    vx = int.from_bytes(data[20:24], byteorder="little", signed=True)
    vy = int.from_bytes(data[24:28], byteorder="little", signed=True)
    size = int.from_bytes(data[28:32], byteorder="little", signed=True)
    heightmap_bytes_len = int.from_bytes(data[32:36], byteorder="little", signed=False)
    biome_size = int.from_bytes(data[36:40], byteorder="little", signed=True)
    biome_bytes_len = int.from_bytes(data[40:44], byteorder="little", signed=False)

    header_size = 52
    heightmap_start = header_size
    heightmap_end = heightmap_start + heightmap_bytes_len
    biome_start = heightmap_end

    heightmap_bytes = data[heightmap_start:heightmap_end]
    heightmap = np.frombuffer(heightmap_bytes, dtype=np.uint16)
    heightmap = heightmap.reshape((vy, vx))

    biome_bytes = data[biome_start : biome_start + biome_bytes_len]
    biome_data = np.frombuffer(biome_bytes, dtype=np.uint8)

    biome_data = biome_data.reshape((vy, vx))

    # Print header information
    print(f"Seed: {seed}, CX: {cx}, CY: {cy}, NumV: {num_v}, VX: {vx}, VY: {vy}")
    print(f"Size: {size}, Heightmap Bytes Len: {heightmap_bytes_len}")
    print(f"Biome Size (bits): {biome_size}, Biome Bytes Len: {biome_bytes_len}")

    plt.figure(figsize=(12, 6))

    plt.subplot(1, 2, 1)
    plt.imshow(heightmap, cmap="terrain", vmin=0, vmax=65535)
    plt.title(f"Heightmap (Seed: {seed}, CX: {cx}, CY: {cy})")
    plt.colorbar(label="Height")

    plt.subplot(1, 2, 2)
    plt.imshow(biome_data, cmap="viridis", vmin=0, vmax=10)
    plt.title(f"Biome Data (bits per pixel: {biome_size})")
    plt.colorbar(label="Biome ID")

    plt.tight_layout()
    plt.show()

    return heightmap


def parse_packet2(packed_data):
    """Parses a binary packet containing heightmap, biome data, and tree placement data.

    Args:
        packed_data: The binary data to parse.

    Returns:
        heightmap: The heightmap data as a 2D numpy array.
        biome_data: The biome data as a 2D numpy array.
        tree_placements: The tree placements as a 1D numpy array.
    """
    header_format = "liiiiiiIiIiI"
    header_size = struct.calcsize(header_format)

    # Split header from the rest
    header_data = packed_data[:header_size]
    body_data = packed_data[header_size:]

    # Unpack header
    (seed, cx, cy, num_v, vx, vy, size, heightmap_len, biome_size, biome_len, size2, tree_placements_len) = (
        struct.unpack(header_format, header_data)
    )

    # Now slice out the rest
    offset = 0

    # Heightmap (uint16)
    heightmap_bytes = body_data[offset : offset + heightmap_len]
    heightmap = np.frombuffer(heightmap_bytes, dtype=np.uint16).reshape((vy, vx))
    offset += heightmap_len

    # Biome data (uint8)s
    biome_bytes = body_data[offset : offset + biome_len]
    biome_data = np.frombuffer(biome_bytes, dtype=np.uint8).reshape((vy, vx))
    offset += biome_len

    # Tree placements (float16)
    tree_placements_bytes = body_data[offset : offset + tree_placements_len]
    tree_placements = np.frombuffer(tree_placements_bytes, dtype=np.float16)

    return heightmap, biome_data, tree_placements
