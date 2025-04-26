import numpy as np
import pytest

from world_generation.biomes.climate_map import classify_biome, determine_biomes, determine_subbiome, in_polygon, zero_preserving_softmax
from world_generation.biomes.create_voronoi import get_chunk_polygons
from world_generation.biomes.land_water_map import determine_landmass
from world_generation.biomes.midpoint_displacement import midpoint_displacement


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
    seed = 42
    polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, polygon_centers = get_chunk_polygons(
        chunk_coords=coord,
        seed=seed,
        chunk_size=size,
        parameters=parameters)

    strength_factors = [0.2, 0.3, 0.3, 0.4, 0.4]
    for strength in strength_factors:
        polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids = midpoint_displacement(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, strength=strength)

    polygon_edges_global_space, polygon_points_local_space, land_water_ids, slice_parts, polygon_points_global_space, offsets = determine_landmass(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, coord, seed, polygon_centers, parameters)

    assert polygon_edges_global_space is not None, "Polygon edges in global space are None"
    return polygon_edges_global_space, polygon_points_local_space, land_water_ids, slice_parts, polygon_points_global_space, offsets, coord

@pytest.fixture
def assigned_biomes_no_params(polygons_no_params, parameters):
    """Fixture to assign biomes to a given set of polygons."""
    coord = polygons_no_params[6]
    polygon_edges = polygons_no_params[0]
    polygon_points = polygons_no_params[1]

    landmass_classifications = polygons_no_params[2]

    offsets = polygons_no_params[5]

    biome_assignment, _ = determine_biomes(
        chunk_coords=coord,
        polygon_edges=polygon_edges,
        polygon_points=polygon_points,
        landmass_classifications=landmass_classifications,
        offsets=offsets,
        parameters=parameters,
        seed=42)
    assert biome_assignment is not None, "Biome assignment failed"
    return biome_assignment, coord, polygon_edges, polygon_points, landmass_classifications, offsets

@pytest.fixture
def polygons_no_params_second(parameters):
    """Fixture to create a set of polygons for testing."""
    # Example coordinates for polygons
    coord = (0, 0)
    size = 1023
    seed = 42
    polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, polygon_centers = get_chunk_polygons(
        chunk_coords=coord,
        seed=seed,
        chunk_size=size,
        parameters=parameters)

    strength_factors = [0.2, 0.3, 0.3, 0.4, 0.4]
    for strength in strength_factors:
        polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids = midpoint_displacement(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, strength=strength)

    polygon_edges_global_space, polygon_points_local_space, land_water_ids, slice_parts, polygon_points_global_space, offsets = determine_landmass(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, coord, seed, polygon_centers, parameters)

    # check test ran
    assert polygon_edges_global_space is not None, "Polygon edges in global space are None"
    return polygon_edges_global_space, polygon_points_local_space, land_water_ids, slice_parts, polygon_points_global_space, offsets, coord

@pytest.fixture
def assigned_biomes_no_params_second(polygons_no_params_second, parameters):
    """Fixture to assign biomes to a given set of polygons."""
    coord = polygons_no_params_second[6]
    polygon_edges = polygons_no_params_second[0]
    polygon_points = polygons_no_params_second[1]

    landmass_classifications = polygons_no_params_second[2]

    offsets = polygons_no_params_second[5]

    biome_assignment, _ = determine_biomes(
        chunk_coords=coord,
        polygon_edges=polygon_edges,
        polygon_points=polygon_points,
        landmass_classifications=landmass_classifications,
        offsets=offsets,
        parameters=parameters,
        seed=42)
    assert biome_assignment is not None, "Biome assignment failed"
    return biome_assignment, coord, polygon_edges, polygon_points, landmass_classifications, offsets

@pytest.fixture
def polygons_no_params_diff_seed(parameters_diff_seed):
    """Fixture to create a set of polygons for testing."""
    # Example coordinates for polygons
    coord = (0, 0)
    size = 1023
    seed = 42
    polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, polygon_centers = get_chunk_polygons(
        chunk_coords=coord,
        seed=seed,
        chunk_size=size,
        parameters=parameters_diff_seed)

    strength_factors = [0.2, 0.3, 0.3, 0.4, 0.4]
    for strength in strength_factors:
        polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids = midpoint_displacement(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, strength=strength)

    polygon_edges_global_space, polygon_points_local_space, land_water_ids, slice_parts, polygon_points_global_space, offsets = determine_landmass(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, coord, seed, polygon_centers, parameters_diff_seed)

    # check test ran
    assert polygon_edges_global_space is not None, "Polygon edges in global space are None"
    return polygon_edges_global_space, polygon_points_local_space, land_water_ids, slice_parts, polygon_points_global_space, offsets, coord

@pytest.fixture
def assigned_biomes_no_params_diff_seed(polygons_no_params_second, parameters_diff_seed):
    """Fixture to assign biomes to a given set of polygons."""
    coord = polygons_no_params_second[6]
    polygon_edges = polygons_no_params_second[0]
    polygon_points = polygons_no_params_second[1]

    landmass_classifications = polygons_no_params_second[2]

    offsets = polygons_no_params_second[5]

    biome_assignment, _ = determine_biomes(
        chunk_coords=coord,
        polygon_edges=polygon_edges,
        polygon_points=polygon_points,
        landmass_classifications=landmass_classifications,
        offsets=offsets,
        parameters=parameters_diff_seed,
        seed=43)
    assert biome_assignment is not None, "Biome assignment failed"
    return biome_assignment, coord, polygon_edges, polygon_points, landmass_classifications, offsets


def test_biomes_assigned(assigned_biomes_no_params):
    biome_assignment = assigned_biomes_no_params[0]
    assert biome_assignment is not None, "Biome assignment is None"


def test_num_biomes(assigned_biomes_no_params):

    polygons = assigned_biomes_no_params[3]
    biome_assignment = assigned_biomes_no_params[0]
    landmass_classifications = assigned_biomes_no_params[4]

    num_polygons = len(polygons)

    # Check if the number of biomes matches the number of polygons

    assert len(biome_assignment) == num_polygons, "Number of polygons does not match number of biomes"

    # check number of polygons matches number of landmass classifications
    assert len(landmass_classifications) == num_polygons, "Number of polygons does not match number of landmass classifications"

def test_biome_ranges(assigned_biomes_no_params):

    biome_assignment = assigned_biomes_no_params[0]

    # check every biome is between 1 and 93

    for biome in biome_assignment:
        assert 1 <= biome <= 93, f"Biome {biome} is out of range (1-93)"

def test_given_same_seed_get_same_biomes(assigned_biomes_no_params, assigned_biomes_no_params_second):
    biome_assignment = assigned_biomes_no_params[0]
    biome_assignment_second = assigned_biomes_no_params_second[0]

    # check that the two biome assignments are the same
    assert np.array_equal(biome_assignment, biome_assignment_second), "Biome assignments are not the same"



def test_given_different_seed_can_get_different_biomes(assigned_biomes_no_params, assigned_biomes_no_params_diff_seed):
    biome_assignment = assigned_biomes_no_params[0]
    biome_assignment_diff_seed = assigned_biomes_no_params_diff_seed[0]

    # check that the two biome assignments are not the same
    assert not np.array_equal(biome_assignment, biome_assignment_diff_seed), "Biome assignments are the same"

def test_zero_preserving_softmax_basic():
    x = [0, 1, 2]
    result = zero_preserving_softmax(x)
    assert result[0] == 0
    assert np.isclose(result[1] + result[2], 1.0, atol=1e-6)


def test_determine_subbiome_boreal():
    seed = 42
    parameters = {
        "boreal_forest": {
            "plains": {"occurrence_probability": 50},
            "hills": {"occurrence_probability": 50},
            "mountains": {"occurrence_probability": 50},
        }
    }
    biome = 1
    choice = determine_subbiome(biome, parameters, seed)
    assert choice in [1, 2, 3]

def test_determine_subbiome_grassland():
    seed = 42
    parameters = {
        "grassland": {
            "plains": {"occurrence_probability": 50},
            "hills": {"occurrence_probability": 50},
            "rocky_fields": {"occurrence_probability": 50},
            "terraced_fields": {"occurrence_probability": 50},
        }
    }
    biome = 10
    choice = determine_subbiome(biome, parameters, seed)
    assert choice in [10, 11, 12, 13]

def test_determine_subbiome_tundra():
    seed = 42
    parameters = {
        "tundra": {
            "plains": {"occurrence_probability": 50},
            "blunt_mountains": {"occurrence_probability": 50},
            "pointy_mountains": {"occurrence_probability": 50},
        }
    }
    biome = 20
    choice = determine_subbiome(biome, parameters, seed)
    assert choice in [20, 21, 22]

def test_determine_subbiome_savanna():
    seed = 42
    parameters = {
        "savanna": {
            "plains": {"occurrence_probability": 50},
            "mountains": {"occurrence_probability": 50},
        }
    }
    biome = 30
    choice = determine_subbiome(biome, parameters, seed)
    assert choice in [30, 31]

def test_determine_subbiome_woodland():
    seed = 42
    parameters = {
        "woodland": {
            "hills": {"occurrence_probability": 100},
        }
    }
    biome = 40
    choice = determine_subbiome(biome, parameters, seed)
    assert choice == 40

def test_determine_subbiome_tropical_rainforest():
    seed = 42
    parameters = {
        "tropical_rainforest": {
            "plains": {"occurrence_probability": 50},
            "mountains": {"occurrence_probability": 50},
            "hills": {"occurrence_probability": 50},
            "volcanoes": {"occurrence_probability": 50},
        }
    }
    biome = 50
    choice = determine_subbiome(biome, parameters, seed)
    assert choice in [50, 51, 52, 53]

def test_determine_subbiome_temperate_rainforest():
    seed = 42
    parameters = {
        "temperate_rainforest": {
            "hills": {"occurrence_probability": 50},
            "mountains": {"occurrence_probability": 50},
            "swamp": {"occurrence_probability": 50},
        }
    }
    biome = 60
    choice = determine_subbiome(biome, parameters, seed)
    assert choice in [60, 61, 62]

def test_determine_subbiome_temperate_seasonal_forest():
    seed = 42
    parameters = {
        "temperate_seasonal_forest": {
            "hills": {"occurrence_probability": 50},
            "mountains": {"occurrence_probability": 50},
        }
    }
    biome = 70
    choice = determine_subbiome(biome, parameters, seed)
    assert choice in [70, 71]

def test_determine_subbiome_subtropical_desert():
    seed = 42
    parameters = {
        "subtropical_desert": {
            "dunes": {"occurrence_probability": 50},
            "mesas": {"occurrence_probability": 50},
            "ravines": {"occurrence_probability": 50},
            "oasis": {"occurrence_probability": 50},
            "cracked": {"occurrence_probability": 50},
        }
    }
    biome = 80
    choice = determine_subbiome(biome, parameters, seed)
    assert choice in [80, 81, 82, 83, 84]

def test_determine_subbiome_ocean():
    seed = 42
    parameters = {
        "ocean": {
            "flat_seabed": {"occurrence_probability": 50},
            "volcanic_islands": {"occurrence_probability": 50},
            "water_stacks": {"occurrence_probability": 50},
            "trenches": {"occurrence_probability": 50},
        }
    }
    biome = 90
    choice = determine_subbiome(biome, parameters, seed)
    assert choice in [90, 91, 92, 93]

def test_classify_biome_grassland():
    parameters = {}
    wanted_biomes = [0, 1, 0, 0, 0, 0, 0, 0, 0]  # only grassland enabled
    temp = -0.05
    precip = -0.1
    biome = classify_biome(temp, precip, parameters, seed=123, wanted_biomes=wanted_biomes)
    assert biome == 10

def test_in_polygon_inside():
    square_x = [0, 0, 10, 10]
    square_y = [0, 10, 10, 0]
    assert in_polygon(4, square_x, square_y, 5, 5) == True
    assert in_polygon(4, square_x, square_y, 15, 5) == False
