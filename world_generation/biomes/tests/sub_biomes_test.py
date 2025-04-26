import numpy as np
import pytest

from world_generation.biomes.sub_biomes import Sub_Biomes


@pytest.fixture
def biomes():
    return Sub_Biomes(seed=42, width=128, height=128, x_offset=0, y_offset=0)

# Helper function for asserting heightmap shape
def assert_heightmap_shape(heightmap, width=128, height=128):
    assert heightmap.shape == (height, width)

# Helper function for asserting heightmap value range
def assert_heightmap_range(heightmap, min_val, max_val):
    assert heightmap.min() >= min_val - 1e-5
    assert heightmap.max() <= max_val + 1e-5

# Helper function for asserting no NaN or None values
def assert_no_nan_or_none(heightmap):
    assert not np.any(np.isnan(heightmap)), "Heightmap contains NaN values"
    assert not np.any(heightmap is None), "Heightmap contains None values"


# Flats Tests
def test_flats_shape(biomes):
    result = biomes.flats(min_height=0.2, max_height=0.8)
    assert_heightmap_shape(result)

def test_flats_range(biomes):
    result = biomes.flats(min_height=0.2, max_height=0.8)
    assert_heightmap_range(result, 0.2, 0.8)

def test_flats_no_nan_or_none(biomes):
    result = biomes.flats(min_height=0.2, max_height=0.8)
    assert_no_nan_or_none(result)

# Hills Tests
def test_hills_shape(biomes):
    result = biomes.hills(min_height=0.3, max_height=0.9)
    assert_heightmap_shape(result)

def test_hills_range(biomes):
    result = biomes.hills(min_height=0.3, max_height=0.9)
    assert_heightmap_range(result, 0.3, 0.9)

def test_hills_no_nan_or_none(biomes):
    result = biomes.hills(min_height=0.3, max_height=0.9)
    assert_no_nan_or_none(result)

# Pointy Peaks Tests
def test_pointy_peaks_shape(biomes):
    result = biomes.pointy_peaks()
    assert_heightmap_shape(result)

def test_pointy_peaks_range(biomes):
    result = biomes.pointy_peaks()
    assert_heightmap_range(result, 0.2, 1)

def test_pointy_peaks_no_nan_or_none(biomes):
    result = biomes.pointy_peaks()
    assert_no_nan_or_none(result)

# Uber Flats Tests
def test_uber_flats_shape(biomes):
    result = biomes.uber_flats()
    assert_heightmap_shape(result)

def test_uber_flats_range(biomes):
    result = biomes.uber_flats()
    assert_heightmap_range(result, 0.2, 0.6)

def test_uber_flats_no_nan_or_none(biomes):
    result = biomes.uber_flats()
    assert_no_nan_or_none(result)

# Volcanoes Tests
def test_volcanoes_shape(biomes):
    result = biomes.volcanoes(min_height=0.1, max_height=0.9)
    assert_heightmap_shape(result)

def test_volcanoes_range(biomes):
    result = biomes.volcanoes(min_height=0.1, max_height=0.9)
    assert_heightmap_range(result, 0.1, 0.9)

def test_volcanoes_no_nan_or_none(biomes):
    result = biomes.volcanoes(min_height=0.1, max_height=0.9)
    assert_no_nan_or_none(result)

# Mesa Terraces Tests
def test_mesa_terraces_shape(biomes):
    result = biomes.mesa_terraces(min_height=0.2, max_height=0.7)
    assert_heightmap_shape(result)

def test_mesa_terraces_range(biomes):
    result = biomes.mesa_terraces(min_height=0.2, max_height=0.7)
    assert_heightmap_range(result, 0.2, 0.7)

def test_mesa_terraces_no_nan_or_none(biomes):
    result = biomes.mesa_terraces(min_height=0.2, max_height=0.7)
    assert_no_nan_or_none(result)

# Dunes Tests
def test_dunes_shape(biomes):
    result = biomes.dunes(min_height=0.0, max_height=1.0)
    assert_heightmap_shape(result)

def test_dunes_range(biomes):
    result = biomes.dunes(min_height=0.0, max_height=1.0)
    assert_heightmap_range(result, 0.0, 1.0)

def test_dunes_no_nan_or_none(biomes):
    result = biomes.dunes(min_height=0.0, max_height=1.0)
    assert_no_nan_or_none(result)

# Phasor Dunes Tests
def test_phasor_dunes_shape(biomes):
    result = biomes.phasor_dunes()
    assert_heightmap_shape(result)

def test_phasor_dunes_range(biomes):
    result = biomes.phasor_dunes()
    assert 0 <= result.min() <= result.max() <= 1

def test_phasor_dunes_no_nan_or_none(biomes):
    result = biomes.phasor_dunes()
    assert_no_nan_or_none(result)

# Extreme Height Range Test
def test_extreme_height_range_shape(biomes):
    result = biomes.flats(min_height=0.5, max_height=0.5)
    assert_heightmap_shape(result)

def test_extreme_height_range_range(biomes):
    result = biomes.flats(min_height=0.5, max_height=0.5)
    assert_heightmap_range(result, 0.5, 0.5)

def test_extreme_height_range_no_nan_or_none(biomes):
    result = biomes.flats(min_height=0.5, max_height=0.5)
    assert_no_nan_or_none(result)

# Large Map Tests
def test_large_map_shape(biomes):
    large_biomes = Sub_Biomes(seed=42, width=512, height=512, x_offset=0, y_offset=0)
    result = large_biomes.flats(min_height=0.2, max_height=0.8)
    assert_heightmap_shape(result, width=512, height=512)

# Min and Max Extreme Values Tests
def test_min_max_values_shape(biomes):
    result = biomes.flats(min_height=-1000, max_height=1000)
    assert_heightmap_shape(result)

def test_min_max_values_range(biomes):
    result = biomes.flats(min_height=-1000, max_height=1000)
    assert_heightmap_range(result, -1000, 1000)

def test_min_max_values_no_nan_or_none(biomes):
    result = biomes.flats(min_height=-1000, max_height=1000)
    assert_no_nan_or_none(result)

# Randomization Consistency Test
def test_randomization_consistency(biomes):
    result1 = biomes.volcanoes(min_height=0.1, max_height=0.9)
    result2 = biomes.volcanoes(min_height=0.1, max_height=0.9)
    assert np.array_equal(result1, result2), "Randomization failed (results differ with the same seed)"

# Rocky Field Tests
def test_rocky_field_shape(biomes):
    result = biomes.rocky_field(min_height=0.2, max_height=0.8)
    assert_heightmap_shape(result)

def test_rocky_field_range(biomes):
    result = biomes.rocky_field(min_height=0.2, max_height=0.8)
    assert_heightmap_range(result, 0.2, 0.8)

def test_rocky_field_no_nan_or_none(biomes):
    result = biomes.rocky_field(min_height=0.2, max_height=0.8)
    assert_no_nan_or_none(result)

# Water Stacks Tests
def test_water_stacks_shape(biomes):
    result = biomes.water_stacks(min_height=0.2, max_height=0.8)
    assert_heightmap_shape(result)

def test_water_stacks_range(biomes):
    result = biomes.water_stacks(min_height=0.2, max_height=0.8)
    assert_heightmap_range(result, 0.2, 0.8)

def test_water_stacks_no_nan_or_none(biomes):
    result = biomes.water_stacks(min_height=0.2, max_height=0.8)
    assert_no_nan_or_none(result)

# Craters Tests
def test_craters_shape(biomes):
    result = biomes.craters()
    assert_heightmap_shape(result)

def test_craters_range(biomes):
    result = biomes.craters()
    assert_heightmap_range(result, 0, 1)

def test_craters_no_nan_or_none(biomes):
    result = biomes.craters()
    assert_no_nan_or_none(result)

# Terraced Rice Fields Tests
def test_terraced_rice_fields_shape(biomes):
    result = biomes.terraced_rice_fields(min_height=0.2, max_height=0.8)
    assert_heightmap_shape(result)

def test_terraced_rice_fields_range(biomes):
    result = biomes.terraced_rice_fields(min_height=0.2, max_height=0.8)
    assert_heightmap_range(result, 0.2, 0.8)

def test_terraced_rice_fields_no_nan_or_none(biomes):
    result = biomes.terraced_rice_fields(min_height=0.2, max_height=0.8)
    assert_no_nan_or_none(result)

# Swamp Tests
def test_swamp_shape(biomes):
    result = biomes.swamp(min_height=0.2, max_height=0.8, wetness=0.5)
    assert_heightmap_shape(result)

def test_swamp_range(biomes):
    result = biomes.swamp(min_height=0.2, max_height=0.8, wetness=0.5)
    assert_heightmap_range(result, 0.2, 0.8)

def test_swamp_no_nan_or_none(biomes):
    result = biomes.swamp(min_height=0.2, max_height=0.8, wetness=0.5)
    assert_no_nan_or_none(result)
