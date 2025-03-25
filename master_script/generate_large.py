import cv2
import numpy as np

from master_script.master_script import main

if __name__ == "__main__":
    kwargs = {
        "biome_size": 10
    }
    effective_zero_x = 200
    effective_zero_y = 200

    large_heightmap = []  # Will store rows

    for i in range(-3, -1):  # Controls vertical stacking
        row_heightmaps = []  # Will store horizontally stacked heightmaps

        for j in range(2, 5):  # Controls horizontal stacking
            heightmap = main(60, effective_zero_x + j, effective_zero_y + i, debug=True, biome=None, **kwargs)
            row_heightmaps.append(heightmap)

        # Stack all heightmaps in a row horizontally
        large_heightmap.append(np.hstack(row_heightmaps))

    # Stack all rows vertically to get final image
    heightmap_rows = np.vstack(large_heightmap)

    print(heightmap_rows.dtype)

    # Save the final image
    cv2.imwrite("master_script/imgs/combined2.png", heightmap_rows)
