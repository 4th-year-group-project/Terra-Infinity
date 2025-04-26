import numpy as np
import pytest

from biomes.create_voronoi import get_chunk_polygons
from biomes.land_water_map import determine_landmass
from biomes.midpoint_displacement import midpoint_displacement


@pytest.fixture
def parameters():
    """Fixture to create a set of parameters for testing."""
    parameters = {
    "mock_data": False,
    "seed": 42,
    "cx": 0,
    "cy": 0,
    "global_max_height": 100,
    "global_tree_density": 50,
    "ocean_coverage": 50,
    "continent_size": 50,
    "biome_size": 50,
    "warmth": 50,
    "wetness": 50,
    "river_frequency": 50,
    "river_width": 50,
    "river_depth": 50,
    "river_meanderiness": 50,
    "debug": False,
    "global_ruggedness": 50,
    "boreal_forest": {
        "selected": True,
        "plains": {
        "max_height": 30,
        "occurrence_probability": 50,
        "evenness": 50,
        "tree_density": 50
        },
        "hills": {
        "max_height": 40,
        "occurrence_probability": 50,
        "bumpiness": 50,
        "tree_density": 50
        },
        "mountains": {
        "max_height": 70,
        "occurrence_probability": 50,
        "ruggedness": 100,
        "tree_density": 50
        }
    },
    "grassland": {
        "selected": True,
        "plains": {
        "max_height": 30,
        "occurrence_probability": 50,
        "evenness": 50,
        "tree_density": 50
        },
        "hills": {
        "max_height": 40,
        "occurrence_probability": 50,
        "bumpiness": 50,
        "tree_density": 50
        },
        "rocky_fields": {
        "max_height": 40,
        "occurrence_probability": 50,
        "rockiness": 50,
        "tree_density": 50
        },
        "terraced_fields": {
        "max_height": 40,
        "occurrence_probability": 30,
        "size": 40,
        "tree_density": 50,
        "smoothness": 50,
        "number_of_terraces": 50
        }
    },
    "tundra": {
        "selected": True,
        "plains": {
        "max_height": 40,
        "occurrence_probability": 50,
        "evenness": 50,
        "tree_density": 50
        },
        "blunt_mountains": {
        "max_height": 100,
        "occurrence_probability": 50,
        "ruggedness": 100,
        "tree_density": 50
        },
        "pointy_mountains": {
        "max_height": 100,
        "occurrence_probability": 50,
        "steepness": 80,
        "frequency": 80,
        "tree_density": 20
        }
    },
    "savanna": {
        "selected": True,
        "plains": {
        "max_height": 30,
        "occurrence_probability": 50,
        "evenness": 50,
        "tree_density": 50
        },
        "mountains": {
        "max_height": 50,
        "occurrence_probability": 50,
        "ruggedness": 100,
        "tree_density": 50
        }
    },
    "woodland": {
        "selected": True,
        "hills": {
        "max_height": 40,
        "occurrence_probability": 50,
        "bumpiness": 50,
        "tree_density": 50
        }
    },
    "tropical_rainforest": {
        "selected": True,
        "plains": {
        "max_height": 40,
        "occurrence_probability": 50,
        "evenness": 50,
        "tree_density": 50
        },
        "mountains": {
        "max_height": 80,
        "occurrence_probability": 50,
        "ruggedness": 100,
        "tree_density": 50
        },
        "hills": {
        "max_height": 50,
        "occurrence_probability": 50,
        "bumpiness": 50,
        "tree_density": 50
        },
        "volcanoes": {
        "max_height": 60,
        "occurrence_probability": 50,
        "size": 100,
        "tree_density": 50,
        "thickness": 60,
        "density": 80
        }
    },
    "temperate_rainforest": {
        "selected": True,
        "hills": {
        "max_height": 40,
        "occurrence_probability": 50,
        "bumpiness": 50,
        "tree_density": 50
        },
        "mountains": {
        "max_height": 80,
        "occurrence_probability": 50,
        "ruggedness": 100,
        "tree_density": 50
        },
        "swamp": {
        "max_height": 30,
        "occurrence_probability": 50,
        "wetness": 80,
        "tree_density": 50
        }
    },
    "temperate_seasonal_forest": {
        "selected": True,
        "hills": {
        "max_height": 40,
        "occurrence_probability": 50,
        "bumpiness": 50,
        "tree_density": 50,
        "autumnal_occurrence": 50
        },
        "mountains": {
        "max_height": 80,
        "occurrence_probability": 50,
        "ruggedness": 100,
        "tree_density": 50,
        "autumnal_occurrence": 50
        }
    },
    "subtropical_desert": {
        "selected": True,
        "dunes": {
        "max_height": 50,
        "occurrence_probability": 50,
        "size": 50,
        "tree_density": 50,
        "dune_frequency": 30,
        "dune_waviness": 80,
        "bumpiness": 50
        },
        "mesas": {
        "max_height": 90,
        "occurrence_probability": 50,
        "size": 50,
        "tree_density": 50,
        "number_of_terraces": 50,
        "steepness": 50
        },
        "ravines": {
        "max_height": 90,
        "occurrence_probability": 50,
        "density": 50,
        "tree_density": 50,
        "ravine_width": 40,
        "smoothness": 50,
        "steepness": 30
        },
        "oasis": {
        "max_height": 30,
        "occurrence_probability": 50,
        "size": 50,
        "flatness": 50,
        "tree_density": 50,
        "dune_frequency": 0
        },
        "cracked": {
        "max_height": 70,
        "occurrence_probability": 50,
        "size": 10,
        "flatness": 50,
        "tree_density": 50
        }
    },
    "ocean": {
        "selected": True,
        "flat_seabed": {
        "max_height": 50,
        "evenness": 50,
        "occurrence_probability": 50
        },
        "volcanic_islands": {
        "max_height": 50,
        "occurrence_probability": 50,
        "size": 50,
        "thickness": 50,
        "density": 50
        },
        "water_stacks": {
        "max_height": 20,
        "occurrence_probability": 50,
        "size": 50
        },
        "trenches": {
        "max_height": 50,
        "density": 10,
        "occurrence_probability": 50,
        "trench_width": 20,
        "smoothness": 20
        }
    }
    }
    return parameters

@pytest.fixture
def parameters_diff_seed(parameters):
    parameters["seed"] = 43
    return parameters

@pytest.fixture
def polygons_no_params(parameters):
    """Fixture to create a set of polygons for testing."""
    # Example coordinates for polygons
    coord = (0, 0)
    size = 1023
    seed = 45
    polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, polygon_centers = get_chunk_polygons(
        chunk_coords=coord,
        seed=seed,
        chunk_size=size,
        parameters=parameters)

    strength_factors = [0.2, 0.3, 0.3, 0.4, 0.4]
    for strength in strength_factors:
        polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids = midpoint_displacement(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, strength=strength)

    return polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, polygon_centers

from biomes.land_water_map import find_polygon_centroid_value, generate_landmass_heights


@pytest.fixture
def polygons_no_params2(parameters):
    """Fixture to create a set of polygons for testing."""
    # Example coordinates for polygons
    coord = (0, 0)
    size = 1023
    seed = 45
    polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, polygon_centers = get_chunk_polygons(
        chunk_coords=coord,
        seed=seed,
        chunk_size=size,
        parameters=parameters)

    strength_factors = [0.2, 0.3, 0.3, 0.4, 0.4]
    for strength in strength_factors:
        polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids = midpoint_displacement(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, strength=strength)

    return polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, polygon_centers


def test_generate_landmass_heights_shape():
    centroids = np.array([[100, 200], [300, 400], [500, 600]])
    seed = 42
    heights = generate_landmass_heights(seed, centroids, scale=5000, sharpness=0.3)
    assert isinstance(heights, np.ndarray)
    assert heights.shape == (3,)
    assert np.all(heights >= -1) and np.all(heights <= 1)

def test_generate_landmass_heights_effect_of_sharpness():
    centroids = np.array([[0, 0], [100, 100], [200, 200]])
    seed = 42
    h_low = generate_landmass_heights(seed, centroids, sharpness=-0.5)
    h_mid = generate_landmass_heights(seed, centroids, sharpness=0.0)
    h_high = generate_landmass_heights(seed, centroids, sharpness=0.5)
    assert not np.allclose(h_low, h_mid)
    assert not np.allclose(h_high, h_mid)

def test_determine_landmass_output_structure(polygons_no_params, parameters):
    edges, points, shared_edges, ids, centers = polygons_no_params
    output = determine_landmass(edges, points, shared_edges, ids, coords=(0,0), seed=42, polygon_centers=centers, parameters=parameters)

    assert isinstance(output, tuple)
    assert len(output) == 6

    polygon_edges_out, polygons_out, land_water_ids, slice_parts, polygons_og_space, min_coords = output

    assert isinstance(polygon_edges_out, list)
    assert isinstance(polygons_out, list)
    assert isinstance(land_water_ids, list)
    assert isinstance(slice_parts, tuple) and len(slice_parts) == 4
    assert isinstance(polygons_og_space, list)
    assert isinstance(min_coords, tuple) and len(min_coords) == 2
    assert all(id in [0, 1] for id in land_water_ids)

def test_find_polygon_centroid_value():
    binary_image = np.zeros((10, 10), dtype=int)
    binary_image[5, 5] = 1

    polygon = [(4, 4), (6, 4), (6, 6), (4, 6)]
    val = find_polygon_centroid_value(polygon.copy(), x_min=0, y_min=0, binary_image=binary_image)
    assert val == 1

def test_determine_landmass_output_same_seed_consistency(polygons_no_params, polygons_no_params2, parameters):
    edges, points, shared_edges, ids, centers = polygons_no_params
    edges2, points2, shared_edges2, ids2, centers2 = polygons_no_params2
    output1 = determine_landmass(edges, points, shared_edges, ids, coords=(0,0), seed=45, polygon_centers=centers, parameters=parameters)
    output2 = determine_landmass(edges2, points2, shared_edges2, ids2, coords=(0,0), seed=45, polygon_centers=centers2, parameters=parameters)
    #assert all are equal

    o11 = output1[0]
    o12 = output2[0]
    o21 = output1[1]
    o22 = output2[1]
    o31 = output1[2]
    o32 = output2[2]
    o41 = output1[3]
    o42 = output2[3]
    o51 = output1[4]
    o52 = output2[4]

    flat_o11 = np.concatenate([np.array(o11[i]).flatten() for i in range(len(o11))])
    flat_o12 = np.concatenate([np.array(o12[i]).flatten() for i in range(len(o12))])

    flat_o21 = np.concatenate([np.array(o21[i]).flatten() for i in range(len(o21))])
    flat_o22 = np.concatenate([np.array(o22[i]).flatten() for i in range(len(o22))])

    flat_o31 = np.concatenate([np.array(o31[i]).flatten() for i in range(len(o31))])
    flat_o32 = np.concatenate([np.array(o32[i]).flatten() for i in range(len(o32))])

    flat_o41 = np.concatenate([np.array(o41[i]).flatten() for i in range(len(o41))])
    flat_o42 = np.concatenate([np.array(o42[i]).flatten() for i in range(len(o42))])

    flat_o51 = np.concatenate([np.array(o51[i]).flatten() for i in range(len(o51))])
    flat_o52 = np.concatenate([np.array(o52[i]).flatten() for i in range(len(o52))])

    assert np.allclose(flat_o11, flat_o12)
    assert np.allclose(flat_o21, flat_o22)
    assert np.allclose(flat_o31, flat_o32)
    assert np.allclose(flat_o41, flat_o42)
    assert np.allclose(flat_o51, flat_o52)



