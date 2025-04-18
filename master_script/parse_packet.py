import matplotlib.pyplot as plt
import numpy as np

# with open("master_script/dump/13_22_3.bin", "rb") as f:
#     data = f.read()

def parse_packet(data):
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
    tree_size = int.from_bytes(data[44:48], byteorder="little", signed=True)
    tree_bytes_len = int.from_bytes(data[48:52], byteorder="little", signed=False)

    header_size = 52
    heightmap_start = header_size
    heightmap_end = heightmap_start + heightmap_bytes_len
    biome_start = heightmap_end

    heightmap_bytes = data[heightmap_start:heightmap_end]
    heightmap = np.frombuffer(heightmap_bytes, dtype=np.uint16)
    heightmap = heightmap.reshape((vy, vx))

    biome_bytes = data[biome_start:biome_start + biome_bytes_len]
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

#parse_packet(data)