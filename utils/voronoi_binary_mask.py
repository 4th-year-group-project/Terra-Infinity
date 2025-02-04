from PIL import Image, ImageDraw
import numpy as np
import matplotlib.pyplot as plt
import cv2

def polygon_to_tight_binary_image(polygon, padding=370, img_size=4000):
    """
    Create a binary image for a polygon with the original bounding box and padding around it.
    The polygon is mapped to a 4000x4000 grid, with its position adjusted based on the original coordinates.
    
    Parameters:
    - polygon: Array of vertices defining the polygon.
    - padding: The amount of padding to add around the bounding box (default is 170).
    - img_size: The size of the final image (default is 4000).
    
    Returns:
    - binary_image_np: Binary image with polygon.
    - (min_x, min_y): Top-left corner of the polygon relative to the final image.
    """
    # Get the bounding box of the polygon
    min_x, min_y = np.round(np.min(polygon, axis=0)).astype(int)
    max_x, max_y = np.round(np.max(polygon, axis=0)).astype(int)
    
    # Compute the width and height of the bounding box
    width = max_x - min_x
    height = max_y - min_y
    
    # Add padding to the bounding box
    min_x -= padding
    min_y -= padding
    width += padding
    height += padding
    
    # Ensure the bounding box is square (optional)
    side_length = max(width, height)
    
    # Find the minimum x and y values of the polygon and apply the offset
    offset_x = min_x + 1524
    offset_y = min_y + 1524

    uffset_x = -(min_x + padding/2)
    uffset_y = -(min_y + padding/2)
    
    # Create a blank binary image with the size of the new bounding box with padding
    binary_image = Image.new("1", (int(side_length), int(side_length)), 0)
    draw = ImageDraw.Draw(binary_image)
    
    # Adjust the polygon coordinates relative to the new (min_x, min_y)
    polygon_tuples = [(coord[0] + uffset_x, coord[1] + uffset_y) for coord in polygon]
    
    # print(f"Adjusted Polygon Tuples: {polygon_tuples}")
    
    # Draw the polygon (fill with white)
    draw.polygon(polygon_tuples, outline=1, fill=1)
    
    # Convert to numpy array
    binary_image_np = np.array(binary_image, dtype=np.uint8)
    
    # plt.imshow(binary_image_np)
    # plt.show()

    # Return the binary image and the top-left corner of the bounding box (relative to the 4000x4000 array)

    #scale up the image by a scale factor of factor of 1.2
    binary_image_np_new = cv2.resize(binary_image_np, (0,0), fx=1.1, fy=1.1)
    offset_x = offset_x - (binary_image_np_new.shape[1] - binary_image_np.shape[1])//2
    offset_y = offset_y - (binary_image_np_new.shape[0] - binary_image_np.shape[0])//2
    return binary_image_np_new, (offset_x, offset_y)