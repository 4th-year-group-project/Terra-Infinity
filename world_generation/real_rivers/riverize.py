import numpy as np

from world_generation.generation import Noise, tools

from .carving import carve_smooth_river_into_terrain, mask_splines, remove_padding


def riverize(heightmap, coords, parameters, river_network):
    cx, cy = coords
    spline_refs = river_network.get_splines_near(cx, cy)

    nearby_edges = set()
    if len(spline_refs) > 0:
        for _tree_id, edge in spline_refs:
            nearby_edges.add(edge)

        splines = []
        for tree_id, edge in spline_refs:
            tree_params = river_network.tree_params[tree_id]
            ts = river_network.tree_splines[tree_id]
            t = {
                "river_width": tree_params["river_width"],
                "scale_exponent": tree_params["scale_exponent"],
                "edge": edge,
            }
            t["spline_points"] = ts.get_bezier_points(ts.control_points[edge], num_points=1000)
            splines.append(t)

        min_x = cx * 1023 - 1
        min_y = cy * 1023 - 1
        max_x = min_x + 1023 + 2
        max_y = min_y + 1023 + 2

        river_mask, padding_info = mask_splines(
            splines, river_network.strahler_numbers, min_x, min_y, max_x, max_y, padding=100
        )
        padding, original_width, original_height = padding_info

        max_width = river_network.max_river_width

        noise_generator = Noise(parameters.get("seed", 0))

        river_depth_pct = parameters["river_depth"]
        river_depth = tools.map0100(100 - river_depth_pct, 0.01, 0.18)

        river_noise = noise_generator.fractal_simplex_noise(
            noise="open",
            height=1026 + 2 * padding,
            width=1026 + 2 * padding,
            x_offset=min_x - padding,
            y_offset=min_y - padding,
            scale=200,
            octaves=7,
            lacunarity=2,
            persistence=0.5,
        )
        river_noise = np.abs(river_noise)

        new_heightmap = carve_smooth_river_into_terrain(
            heightmap,
            river_mask,
            max_width,
            river_noise=river_noise,
            noise_strength=0.2,
            river_depth_factor=river_depth,
        )

        new_heightmap = remove_padding(new_heightmap, padding, original_width, original_height)

        return new_heightmap

    else:
        return remove_padding(heightmap, 100, 1026, 1026)
