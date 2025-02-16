import numpy as np
import matplotlib.pyplot as plt

with open("output.bin", "rb") as f:
    data = f.read()

seed = int.from_bytes(data[0:8], byteorder='little', signed=True)
cx = int.from_bytes(data[8:16], byteorder='little', signed=True)
cy = int.from_bytes(data[12:16], byteorder='little', signed=True)
num_v = int.from_bytes(data[16:20], byteorder='little', signed=True)
size = int.from_bytes(data[20:24], byteorder='little', signed=True)
heightmap_bytes_len = int.from_bytes(data[24:28], byteorder='little', signed=False)

# Extract heightmap data
heightmap_bytes = data[28:]  # Remaining bytes
heightmap = np.frombuffer(heightmap_bytes, dtype=np.uint16)

# Reshape it correctly


# Print and display the heightmap
print(f"Seed: {seed}, CX: {cx}, CY: {cy}, NumV: {num_v}, Size: {size}, Heightmap Bytes Len: {heightmap_bytes_len}")
heightmap = heightmap.reshape((1026,1026))
print(heightmap)

plt.imshow(heightmap, cmap='gray')
plt.title(f"Heightmap (Seed: {seed}, CX: {cx}, CY: {cy})")
plt.show()