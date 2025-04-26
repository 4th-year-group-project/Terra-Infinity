import numpy as np
import pytest

from world_generation.biomes.create_voronoi import get_chunk_polygons, get_polygons


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
            "plains": {"max_height": 30, "occurrence_probability": 50, "evenness": 50, "tree_density": 50},
            "hills": {"max_height": 40, "occurrence_probability": 50, "bumpiness": 50, "tree_density": 50},
            "mountains": {"max_height": 70, "occurrence_probability": 50, "ruggedness": 100, "tree_density": 50},
        },
        "grassland": {
            "selected": True,
            "plains": {"max_height": 30, "occurrence_probability": 50, "evenness": 50, "tree_density": 50},
            "hills": {"max_height": 40, "occurrence_probability": 50, "bumpiness": 50, "tree_density": 50},
            "rocky_fields": {"max_height": 40, "occurrence_probability": 50, "rockiness": 50, "tree_density": 50},
            "terraced_fields": {
                "max_height": 40,
                "occurrence_probability": 30,
                "size": 40,
                "tree_density": 50,
                "smoothness": 50,
                "number_of_terraces": 50,
            },
        },
        "tundra": {
            "selected": True,
            "plains": {"max_height": 40, "occurrence_probability": 50, "evenness": 50, "tree_density": 50},
            "blunt_mountains": {"max_height": 100, "occurrence_probability": 50, "ruggedness": 100, "tree_density": 50},
            "pointy_mountains": {
                "max_height": 100,
                "occurrence_probability": 50,
                "steepness": 80,
                "frequency": 80,
                "tree_density": 20,
            },
        },
        "savanna": {
            "selected": True,
            "plains": {"max_height": 30, "occurrence_probability": 50, "evenness": 50, "tree_density": 50},
            "mountains": {"max_height": 50, "occurrence_probability": 50, "ruggedness": 100, "tree_density": 50},
        },
        "woodland": {
            "selected": True,
            "hills": {"max_height": 40, "occurrence_probability": 50, "bumpiness": 50, "tree_density": 50},
        },
        "tropical_rainforest": {
            "selected": True,
            "plains": {"max_height": 40, "occurrence_probability": 50, "evenness": 50, "tree_density": 50},
            "mountains": {"max_height": 80, "occurrence_probability": 50, "ruggedness": 100, "tree_density": 50},
            "hills": {"max_height": 50, "occurrence_probability": 50, "bumpiness": 50, "tree_density": 50},
            "volcanoes": {
                "max_height": 60,
                "occurrence_probability": 50,
                "size": 100,
                "tree_density": 50,
                "thickness": 60,
                "density": 80,
            },
        },
        "temperate_rainforest": {
            "selected": True,
            "hills": {"max_height": 40, "occurrence_probability": 50, "bumpiness": 50, "tree_density": 50},
            "mountains": {"max_height": 80, "occurrence_probability": 50, "ruggedness": 100, "tree_density": 50},
            "swamp": {"max_height": 30, "occurrence_probability": 50, "wetness": 80, "tree_density": 50},
        },
        "temperate_seasonal_forest": {
            "selected": True,
            "hills": {
                "max_height": 40,
                "occurrence_probability": 50,
                "bumpiness": 50,
                "tree_density": 50,
                "autumnal_occurrence": 50,
            },
            "mountains": {
                "max_height": 80,
                "occurrence_probability": 50,
                "ruggedness": 100,
                "tree_density": 50,
                "autumnal_occurrence": 50,
            },
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
                "bumpiness": 50,
            },
            "mesas": {
                "max_height": 90,
                "occurrence_probability": 50,
                "size": 50,
                "tree_density": 50,
                "number_of_terraces": 50,
                "steepness": 50,
            },
            "ravines": {
                "max_height": 90,
                "occurrence_probability": 50,
                "density": 50,
                "tree_density": 50,
                "ravine_width": 40,
                "smoothness": 50,
                "steepness": 30,
            },
            "oasis": {
                "max_height": 30,
                "occurrence_probability": 50,
                "size": 50,
                "flatness": 50,
                "tree_density": 50,
                "dune_frequency": 0,
            },
            "cracked": {"max_height": 70, "occurrence_probability": 50, "size": 10, "flatness": 50, "tree_density": 50},
        },
        "ocean": {
            "selected": True,
            "flat_seabed": {"max_height": 50, "evenness": 50, "occurrence_probability": 50},
            "volcanic_islands": {
                "max_height": 50,
                "occurrence_probability": 50,
                "size": 50,
                "thickness": 50,
                "density": 50,
            },
            "water_stacks": {"max_height": 20, "occurrence_probability": 50, "size": 50},
            "trenches": {
                "max_height": 50,
                "density": 10,
                "occurrence_probability": 50,
                "trench_width": 20,
                "smoothness": 20,
            },
        },
    }
    return parameters


@pytest.fixture
def polygons(parameters):
    chunk_coords = (0, 0)
    chunk_size = 1023
    seed = 42

    polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, polygon_centers = (
        get_chunk_polygons(chunk_coords, seed, chunk_size, parameters)
    )

    return polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, polygon_centers


def test_get_chunk_polygons(polygons):
    # check each return is not None

    assert polygons[0] is not None, "Polygon edges should not be None"
    assert polygons[1] is not None, "Polygon points should not be None"
    assert polygons[2] is not None, "Shared edges should not be None"
    assert polygons[3] is not None, "Polygon IDs should not be None"
    assert polygons[4] is not None, "Polygon centers should not be None"


def test_get_polygons_simple_grid():
    points = [[0, 0], [0, 512], [512, 0], [1023, 1023]]
    region_polygons, vor, shared_edges, polygon_points, polygon_centers = get_polygons(points)

    assert len(region_polygons) == len(polygon_points) == len(polygon_centers), (
        "Mismatch in number of regions, points, and centers"
    )


def test_infinite_regions_are_skipped():
    points = [[0, 0], [10, 0], [5, 1000]]
    region_polygons, _, _, _, _ = get_polygons(points)

    for region in region_polygons:
        for edge in region:
            for vertex in edge:
                assert -1 not in vertex, "Region contains infinite edges"


def test_shared_edges_tracking():
    points = [[0, 0], [0, 10], [10, 0], [10, 10]]
    _, _, shared_edges, _, _ = get_polygons(points)

    assert isinstance(shared_edges, dict), "Shared edges should be a dictionary"
    assert all(isinstance(k, tuple) for k in shared_edges), "Keys should be tuples"


def test_random_points_voronoi():
    np.random.seed(42)
    points = np.random.rand(50, 2) * 100
    result = get_polygons(points)
    region_polygons, _, shared_edges, polygon_points, _ = result

    assert len(region_polygons) == len(polygon_points), "Mismatch in number of regions and points"
    assert isinstance(shared_edges, dict), "Shared edges should be a dictionary"
