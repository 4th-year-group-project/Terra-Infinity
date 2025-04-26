import numpy as np
import pytest

from world_generation.cellular_automata.CA import Growth_And_Crowding_CA


@pytest.fixture
def ca():
    """Fixture to create a Growth_And_Crowding_CA instance for testing."""
    size = 100
    initial_grid = np.zeros((size, size))
    initial_grid[size // 2, size // 2] = 1

    return Growth_And_Crowding_CA(
        size=size,
        growth_threshold=2500,
        initial_food=100,
        food_algorithm="Diffuse",
        eat_value=5,
        steps_between_growth=3,
        delta=0.5,
        initial_life_grid=initial_grid,
        food_mask=np.ones((size, size), bool),
        seed=42,
    )

@pytest.fixture
def fullrun_ca():
    """Fixture to create a Growth_And_Crowding_CA instance for testing."""
    size = 100
    initial_grid = np.zeros((size, size))
    initial_grid[size // 2, size // 2] = 1

    return Growth_And_Crowding_CA(
        size=size,
        growth_threshold=2500,
        initial_food=100,
        food_algorithm="Diffuse",
        eat_value=15,
        steps_between_growth=3,
        delta=0.79,
        initial_life_grid=initial_grid,
        food_mask=np.ones((size, size), bool),
        seed=42,
    )

def test_initialization(ca):
    """Test if the class initializes correctly."""
    assert ca.size == 100
    assert ca.life_grid.shape == (100, 100)
    assert ca.food_grid.shape == (100, 100)
    assert np.count_nonzero(ca.life_grid) == 1  # Ensure one initial cell

def test_step_updates_time(ca):
    """Test if the step function updates time correctly."""
    initial_time = ca.time
    ca.step()
    assert ca.time == initial_time + 1

def test_apply_food_rule(ca):
    """Ensure the food rule updates the food grid."""
    initial_food = ca.food_grid.copy()
    initial_food[ca.size // 2, ca.size // 2] = 0
    ca.apply_food_rule()
    assert not np.array_equal(initial_food, ca.food_grid)

def test_life_growth(ca):
    """Test that life can grow under correct conditions."""
    ca.apply_life_rule()
    assert np.count_nonzero(ca.new_life_grid) >= np.count_nonzero(ca.life_grid)

def test_life_eats_food(ca):
    """Test if life eats food correctly."""
    initial_food = ca.food_grid.copy()
    ca.life_eats_food()
    assert np.any(ca.food_grid < initial_food)

def test_diffuse_food(ca):
    """Ensure food diffusion algorithm works."""
    initial_food = ca.food_grid.copy()
    initial_food[ca.size // 2, ca.size // 2] = 0
    new_food = ca.diffuse()
    assert not np.array_equal(initial_food, new_food)

def test_average_food(ca):
    """Ensure average food calculation is valid."""
    avg_food = ca.average_food()
    assert avg_food.shape == ca.food_grid.shape

def test_count_alive_neighbours(ca):
    """Ensure neighbour count function works."""
    count = ca.count_alive_neighbours()
    assert count.shape == ca.life_grid.shape
    assert np.all(count >= 0)

def test_update_directions(ca):
    """Ensure update directions assigns new values correctly."""
    ca.update_directions(np.ones((100, 100), dtype=bool))
    assert np.any(ca.direction_grid != 0)

def test_full_run(fullrun_ca):
    """Test a run of the CA for 100 steps against a known result."""
    known_life_grid = np.load("world_generation/cellular_automata/tests/data/life_grid.npy")
    known_food_grid = np.load("world_generation/cellular_automata/tests/data/food_grid.npy")
    steps = 100
    for _step in range(steps):
        fullrun_ca.step()

    assert np.array_equal(fullrun_ca.life_grid, known_life_grid)
    assert np.array_equal(fullrun_ca.food_grid, known_food_grid)
