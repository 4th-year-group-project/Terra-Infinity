import numpy as np
from PIL import Image, ImageDraw


def polygon_to_tight_binary_image(polygon, padding=370):
    """Create a binary image for a polygon with the original bounding box and padding around it.

    The polygon is mapped to a 4000x4000 grid, with its position adjusted based on the original coordinates.

    Parameters:
    - polygon: Array of vertices defining the polygon.
    - padding: The amount of padding to add around the bounding box (default is 170).
    - img_size: The size of the final image (default is 4000).

    Returns:
    - binary_image_np: Binary image with polygon.
    - (min_x, min_y): Top-left corner of the polygon relative to the final image.
    """
    min_x, min_y = np.round(np.min(polygon, axis=0)).astype(int)
    max_x, max_y = np.round(np.max(polygon, axis=0)).astype(int)

    width = max_x - min_x
    height = max_y - min_y

    min_x -= padding
    min_y -= padding
    width += padding
    height += padding

    side_length = max(width, height)

    offset_x = min_x + 370
    offset_y = min_y + 370

    uffset_x = -(min_x + padding / 2)
    uffset_y = -(min_y + padding / 2)

    binary_image = Image.new("1", (int(side_length), int(side_length)), 0)
    draw = ImageDraw.Draw(binary_image)

    polygon_tuples = [(coord[0] + uffset_x, coord[1] + uffset_y) for coord in polygon]
    draw.polygon(polygon_tuples, outline=1, fill=1)

    binary_image_np = np.array(binary_image, dtype=np.uint8)

    return binary_image_np, (offset_x, offset_y)
