
import numpy as np

from world_generation.biomes.midpoint_displacement import consistent_normal, cycle_array, midpoint_displacement, stable_seed


# --------- stable_seed Tests ---------
def test_stable_seed_consistency():
    v1 = (1.1, 2.2)
    v2 = (3.3, 4.4)
    seed1 = stable_seed(v1, v2)
    seed2 = stable_seed(v2, v1)
    assert seed1 == seed2, "Seed should be consistent regardless of vertex order"

def test_stable_seed_determinism():
    v1 = (5.0, 5.0)
    v2 = (6.0, 6.0)
    assert stable_seed(v1, v2) == stable_seed(v1, v2), "Seed should be deterministic"

# --------- consistent_normal Tests ---------
def test_consistent_normal_output():
    v1 = np.array([0.0, 0.0])
    v2 = np.array([1.0, 0.0])
    normal = consistent_normal(v1, v2)
    expected = np.array([0.0, 1.0])  # Normal to rightward vector should point up
    np.testing.assert_array_almost_equal(normal, expected, decimal=5)

def test_normal_consistency():
    v1 = np.array([2.0, 1.0])
    v2 = np.array([3.0, 4.0])
    n1 = consistent_normal(v1, v2)
    n2 = consistent_normal(v2, v1)
    np.testing.assert_array_almost_equal(n1, n2, decimal=5)

# --------- cycle_array Tests ---------
def test_cycle_array_rotation():
    polygon = [[3, 0], [5, 0], [4, 2]]
    cycled = cycle_array(polygon)
    assert cycled[0][0] == min(p[0] for p in polygon), "First x-coordinate should be the minimum"

# --------- midpoint_displacement Tests ---------
def test_midpoint_displacement_shape_and_structure():
    polygon = [np.array([[0, 0], [1, 0], [1, 1], [0, 1]])]
    edges, output, shared, ids = midpoint_displacement([], polygon, [], [0])
    assert len(output) == 1
    assert len(output[0]) == 8, "Each side should add a midpoint, doubling the number of vertices"

def test_midpoint_displacement_repeatability():
    polygon = [np.array([[0, 0], [1, 0], [1, 1], [0, 1]])]
    _, output1, _, _ = midpoint_displacement([], polygon, [], [0])
    _, output2, _, _ = midpoint_displacement([], polygon, [], [0])
    np.testing.assert_array_almost_equal(output1[0], output2[0], decimal=5)

