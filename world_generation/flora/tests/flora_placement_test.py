import numpy as np
import pytest
from scipy.ndimage import sobel

from world_generation.flora.flora_placement import apply_sobel, find_intersections, get_vegetation_map, packing, place_plants


def test_find_intersections():
    # Circle 1 at (0, 0) with radius 5
    # Circle 2 at (4, 0) with radius 5 (should intersect at two points)
    circle1_centre = (0, 0)
    circle2_centre = (4, 0)
    circle1_radius = 5
    circle2_radius = 5
    point1, point2 = find_intersections(circle1_centre, circle2_centre, circle1_radius, circle2_radius)

    # Check if the intersection points are valid
    assert point1 is not None, "Point1 should not be None"
    assert point2 is not None, "Point2 should not be None"

    # Check if points lie within the circle bounds
    assert np.sqrt(point1[0]**2 + point1[1]**2) <= circle1_radius, "Point1 is outside Circle 1"
    assert np.sqrt(point2[0]**2 + point2[1]**2) <= circle2_radius, "Point2 is outside Circle 2"

# Test `packing` function
def test_packing():
    seed = 42
    min_x, max_x = 0, 1024
    min_y, max_y = 0, 1024
    chunk_size = 1024
    sparseness = 5
    points = packing(seed, min_x, max_x, min_y, max_y, chunk_size, sparseness)

    # Test if the points are within the bounds
    for point in points:
        x, y = point
        assert min_x <= x <= max_x, "X coordinate out of bounds"
        assert min_y <= y <= max_y, "Y coordinate out of bounds"

    # Test if there are points generated (shouldn't be empty)
    assert len(points) > 0, "Expected some points to be generated"

# Test `get_vegetation_map` function
@pytest.fixture
def mock_data():
    width, height = 1024, 1024
    heightmap = np.random.rand(height, width)
    spread_mask = np.random.rand(height, width)
    sobel_h = sobel(heightmap, 0)
    sobel_v = sobel(heightmap, 1)
    noise_map = np.random.rand(height, width)
    return heightmap, spread_mask, sobel_h, sobel_v, noise_map

def test_get_vegetation_map(mock_data):
    heightmap, spread_mask, sobel_h, sobel_v, noise_map = mock_data
    seed = 42
    vegetation_map = get_vegetation_map(spread_mask, sobel_h, sobel_v, heightmap, seed, noise_map, 1024, 1024)

    # Check if vegetation_map has the correct shape
    assert vegetation_map.shape == (1024, 1024), "Vegetation map shape mismatch"

    # Check if vegetation_map values are between 0 and 1 (as they should be noise values)
    assert np.all(vegetation_map >= 0) and np.all(vegetation_map <= 1), "Vegetation map values should be between 0 and 1"

# Test `apply_sobel` function
@pytest.fixture
def apply_sobel_data():
    heightmap = np.random.rand(1024, 1024)
    spread_mask = np.random.rand(1024, 1024)
    spread = 0.05
    seed = 42
    x_offset = 10
    y_offset = 10
    low = 0
    high = 1
    return heightmap, spread_mask, spread, seed, x_offset, y_offset, low, high

def test_apply_sobel(apply_sobel_data):
    heightmap, spread_mask, spread, seed, x_offset, y_offset, low, high = apply_sobel_data
    mask = apply_sobel(heightmap, spread_mask, spread, seed, x_offset, y_offset, high, low)

    # Check if mask has the correct shape
    assert mask.shape == (1024, 1024), "Mask shape mismatch"

    # Check if values in the mask are between 0 and 1
    assert np.all(mask >= 0) and np.all(mask <= 1), "Mask values should be between 0 and 1"

# Test `place_plants` function
def test_place_plants():
    heightmap = np.random.rand(1024, 1024)
    spread_mask = np.random.rand(1024, 1024)
    seed = 42
    x_offset = 10
    y_offset = 10
    width, height = 1024, 1024
    size = 1024
    spread = 0.05
    sparseness = 5
    coverage = 0.6
    high = 1
    low = 0

    points = place_plants(heightmap, spread_mask, seed, x_offset, y_offset, width, height, size, spread, sparseness, coverage, high, low)

    # Test if the returned points are within the bounds
    for point in points:
        x, y = point
        assert 0 <= x < width, "X coordinate out of bounds"
        assert 0 <= y < height, "Y coordinate out of bounds"

    # Test if there are any points generated
    assert len(points) > 0, "Expected some points to be generated"
