import numpy as np
from noise import snoise2
from numba import njit, prange

### https://github.com/lmas/opensimplex

SIMP_GRAD2 = np.array(
    [
        1,
        1,
        -1,
        1,
        1,
        -1,
        -1,
        -1,
        1,
        0,
        -1,
        0,
        1,
        0,
        -1,
        0,
        0,
        1,
        0,
        -1,
        0,
        1,
        0,
        -1,
    ],
    dtype=np.int64,
)

OPEN_GRAD2 = np.array(
    [
        5,
        2,
        2,
        5,
        -5,
        2,
        -2,
        5,
        5,
        -2,
        2,
        -5,
        -5,
        -2,
        -2,
        -5,
    ],
    dtype=np.int64,
)

SIMP_STRETCH = 0.36602540378
SIMP_SQUASH = 0.2113248654

OPEN_STRETCH = -0.2113248654
OPEN_SQUASH = 0.36602540378


@njit(fastmath=True, cache=True)
def simp_extrapolate(perm, xsb, ysb, dx, dy):
    index = (perm[(perm[xsb & 0xFF] + ysb) & 0xFF] % 12) * 2
    g1, g2 = SIMP_GRAD2[index : index + 2]
    return g1 * dx + g2 * dy


@njit(fastmath=True, cache=True)
def open_extrapolate(perm, xsb, ysb, dx, dy):
    index = perm[(perm[xsb & 0xFF] + ysb) & 0xFF] & 0x0E
    g1, g2 = OPEN_GRAD2[index : index + 2]
    return g1 * dx + g2 * dy


@njit(fastmath=True, cache=True)
def open_extrapolate_gradient(perm, xsb, ysb, dx, dy):
    index = perm[(perm[xsb & 0xFF] + ysb) & 0xFF] & 0x0E
    g1, g2 = OPEN_GRAD2[index : index + 2]
    return np.array([g1 * dx + g2 * dy, g2 * dx - g1 * dy])


@njit(fastmath=True, cache=True)
def noise2(perm, x, y):
    # Skew the input space to determine the simplex cell
    s = (x + y) * SIMP_STRETCH
    xs = x + s
    ys = y + s

    # Calculate the integer grid coords of simplex cell origin
    xsb = int(np.floor(xs))
    ysb = int(np.floor(ys))

    # Unskew the cell back to the original space
    t = (xsb + ysb) * SIMP_SQUASH
    xb = xsb - t
    yb = ysb - t

    # Calculate distances from cell origin
    x0 = x - xb
    y0 = y - yb

    # Determine simplex region we are in
    if x0 > y0:
        i1, j1 = 1, 0  # Lower triangle, XY order
    else:
        i1, j1 = 0, 1  # Upper triangle, YX order

    # Calculate x, y distances from other two corners
    x1 = x0 - i1 + SIMP_SQUASH
    y1 = y0 - j1 + SIMP_SQUASH
    x2 = x0 - 1.0 + 2.0 * SIMP_SQUASH
    y2 = y0 - 1.0 + 2.0 * SIMP_SQUASH

    n0, n1, n2 = 0.0, 0.0, 0.0

    # Contribution from (0, 0)
    t0 = 0.5 - x0 * x0 - y0 * y0  # Fade function
    if t0 > 0:
        t0 *= t0
        n0 = t0 * t0 * simp_extrapolate(perm, xsb, ysb, x0, y0)

    # Contribution from (i1, j1)
    t1 = 0.5 - x1 * x1 - y1 * y1  # Fade function
    if t1 > 0:
        t1 *= t1
        n1 = t1 * t1 * simp_extrapolate(perm, xsb + i1, ysb + j1, x1, y1)

    # Contribution from (1, 1)
    t2 = 0.5 - x2 * x2 - y2 * y2  # Fade function
    if t2 > 0:
        t2 *= t2
        n2 = t2 * t2 * simp_extrapolate(perm, xsb + 1, ysb + 1, x2, y2)

    # Sum up and scale the result to be within [-1, 1]
    return 70.0 * (n0 + n1 + n2)


@njit(fastmath=True, cache=True)
def open_noise2(perm, x, y):
    # Skew the input space to determine the simplex cell
    stretch_offset = (x + y) * OPEN_STRETCH
    xs = x + stretch_offset
    ys = y + stretch_offset

    # Calculate the integer grid coords of simplex cell origin
    xsb = int(np.floor(xs))
    ysb = int(np.floor(ys))

    # Unskew the cell back to the original space
    squish_offset = (xsb + ysb) * OPEN_SQUASH
    xb = xsb + squish_offset
    yb = ysb + squish_offset

    # Determine simplex region we are in
    xins = xs - xsb
    yins = ys - ysb
    in_sum = xins + yins

    # Calculate distances from cell origin
    dx0 = x - xb
    dy0 = y - yb

    value = 0

    # Contribution from (1,0)
    dx1 = dx0 - 1 - OPEN_SQUASH
    dy1 = dy0 - 0 - OPEN_SQUASH
    attn1 = 2 - dx1 * dx1 - dy1 * dy1  # Fade function
    if attn1 > 0:
        attn1 *= attn1
        value += attn1 * attn1 * open_extrapolate(perm, xsb + 1, ysb + 0, dx1, dy1)

    # Contribution from (0,1)
    dx2 = dx0 - 0 - OPEN_SQUASH
    dy2 = dy0 - 1 - OPEN_SQUASH
    attn2 = 2 - dx2 * dx2 - dy2 * dy2  # Fade function
    if attn2 > 0:
        attn2 *= attn2
        value += attn2 * attn2 * open_extrapolate(perm, xsb + 0, ysb + 1, dx2, dy2)

    if in_sum <= 1:  # Inside the simplex at (0,0)
        zins = 1 - in_sum
        if zins > xins or zins > yins:  # (0,0) is one of the closest two triangular vertices
            if xins > yins:
                # (1,0) is the other closest vertex
                xsv_ext = xsb + 1
                ysv_ext = ysb - 1
                dx_ext = dx0 - 1
                dy_ext = dy0 + 1
            else:
                # (0,1) is the other closest vertex
                xsv_ext = xsb - 1
                ysv_ext = ysb + 1
                dx_ext = dx0 + 1
                dy_ext = dy0 - 1
        else:  # (1,0) and (0,1) are the closest two vertices
            xsv_ext = xsb + 1
            ysv_ext = ysb + 1
            dx_ext = dx0 - 1 - 2 * OPEN_SQUASH
            dy_ext = dy0 - 1 - 2 * OPEN_SQUASH
    else:  # We're inside the triangle (2-Simplex) at (1,1)
        zins = 2 - in_sum
        if zins < xins or zins < yins:  # (0,0) is one of the closest two triangular vertices
            if xins > yins:
                # (1,0) is the other closest vertex
                xsv_ext = xsb + 2
                ysv_ext = ysb + 0
                dx_ext = dx0 - 2 - 2 * OPEN_SQUASH
                dy_ext = dy0 + 0 - 2 * OPEN_SQUASH
            else:
                # (0,1) is the other closest vertex
                xsv_ext = xsb + 0
                ysv_ext = ysb + 2
                dx_ext = dx0 + 0 - 2 * OPEN_SQUASH
                dy_ext = dy0 - 2 - 2 * OPEN_SQUASH
        else:  # (1,0) and (0,1) are the closest two vertices
            dx_ext = dx0
            dy_ext = dy0
            xsv_ext = xsb
            ysv_ext = ysb

        xsb += 1
        ysb += 1
        dx0 = dx0 - 1 - 2 * OPEN_SQUASH
        dy0 = dy0 - 1 - 2 * OPEN_SQUASH

    # Contribution from (0,0) or (1,1)
    attn0 = 2 - dx0 * dx0 - dy0 * dy0
    if attn0 > 0:
        attn0 *= attn0
        value += attn0 * attn0 * open_extrapolate(perm, xsb, ysb, dx0, dy0)

    # Contribution from (1,0) or (0,1)
    attn_ext = 2 - dx_ext * dx_ext - dy_ext * dy_ext
    if attn_ext > 0:
        attn_ext *= attn_ext
        value += attn_ext * attn_ext * open_extrapolate(perm, xsv_ext, ysv_ext, dx_ext, dy_ext)

    return value / 47.0


@njit(fastmath=True, cache=True)
def open_noise2_grad(perm, x, y):
    # Skew the input space to determine the simplex cell
    stretch_offset = (x + y) * OPEN_STRETCH
    xs = x + stretch_offset
    ys = y + stretch_offset

    # Calculate the integer grid coords of simplex cell origin
    xsb = int(np.floor(xs))
    ysb = int(np.floor(ys))

    # Unskew the cell back to the original space
    squish_offset = (xsb + ysb) * OPEN_SQUASH
    xb = xsb + squish_offset
    yb = ysb + squish_offset

    # Determine simplex region we are in
    xins = xs - xsb
    yins = ys - ysb
    in_sum = xins + yins

    # Calculate distances from cell origin
    dx0 = x - xb
    dy0 = y - yb

    grad_x, grad_y = 0.0, 0.0  # Gradients to accumulate

    # Contribution from (1,0)
    dx1 = dx0 - 1 - OPEN_SQUASH
    dy1 = dy0 - 0 - OPEN_SQUASH
    attn1 = 2 - dx1 * dx1 - dy1 * dy1
    if attn1 > 0:
        attn1 *= attn1
        grad_x += -4 * dx1 * attn1 * open_extrapolate(perm, xsb + 1, ysb + 0, dx1, dy1)
        grad_y += -4 * dy1 * attn1 * open_extrapolate(perm, xsb + 1, ysb + 0, dx1, dy1)

    # Contribution from (0,1)
    dx2 = dx0 - 0 - OPEN_SQUASH
    dy2 = dy0 - 1 - OPEN_SQUASH
    attn2 = 2 - dx2 * dx2 - dy2 * dy2
    if attn2 > 0:
        attn2 *= attn2
        grad_x += -4 * dx2 * attn2 * open_extrapolate(perm, xsb + 0, ysb + 1, dx2, dy2)
        grad_y += -4 * dy2 * attn2 * open_extrapolate(perm, xsb + 0, ysb + 1, dx2, dy2)

    # For inside the simplex at (0,0)
    if in_sum <= 1:
        zins = 1 - in_sum
        if zins > xins or zins > yins:  # (0,0) is one of the closest two triangular vertices
            if xins > yins:
                # (1,0) is the other closest vertex
                xsv_ext = xsb + 1
                ysv_ext = ysb - 1
                dx_ext = dx0 - 1
                dy_ext = dy0 + 1
            else:
                # (0,1) is the other closest vertex
                xsv_ext = xsb - 1
                ysv_ext = ysb + 1
                dx_ext = dx0 + 1
                dy_ext = dy0 - 1
        else:  # (1,0) and (0,1) are the closest two vertices
            xsv_ext = xsb + 1
            ysv_ext = ysb + 1
            dx_ext = dx0 - 1 - 2 * OPEN_SQUASH
            dy_ext = dy0 - 1 - 2 * OPEN_SQUASH
        # Gradient contributions for this part
        attn_ext = 2 - dx_ext * dx_ext - dy_ext * dy_ext
        if attn_ext > 0:
            attn_ext *= attn_ext
            grad_x += -4 * dx_ext * attn_ext * open_extrapolate(perm, xsv_ext, ysv_ext, dx_ext, dy_ext)
            grad_y += -4 * dy_ext * attn_ext * open_extrapolate(perm, xsv_ext, ysv_ext, dx_ext, dy_ext)

    else:  # We're inside the triangle (2-Simplex) at (1,1)
        zins = 2 - in_sum
        if zins < xins or zins < yins:  # (0,0) is one of the closest two triangular vertices
            if xins > yins:
                # (1,0) is the other closest vertex
                xsv_ext = xsb + 2
                ysv_ext = ysb + 0
                dx_ext = dx0 - 2 - 2 * OPEN_SQUASH
                dy_ext = dy0 + 0 - 2 * OPEN_SQUASH
            else:
                # (0,1) is the other closest vertex
                xsv_ext = xsb + 0
                ysv_ext = ysb + 2
                dx_ext = dx0 + 0 - 2 * OPEN_SQUASH
                dy_ext = dy0 - 2 - 2 * OPEN_SQUASH
        else:  # (1,0) and (0,1) are the closest two vertices
            dx_ext = dx0
            dy_ext = dy0
            xsv_ext = xsb
            ysv_ext = ysb

        # Update the simplex coordinates for (1,1)
        xsb += 1
        ysb += 1
        dx0 = dx0 - 1 - 2 * OPEN_SQUASH
        dy0 = dy0 - 1 - 2 * OPEN_SQUASH

        # Gradient contributions for this part
        attn_ext = 2 - dx_ext * dx_ext - dy_ext * dy_ext
        if attn_ext > 0:
            attn_ext *= attn_ext
            grad_x += -4 * dx_ext * attn_ext * open_extrapolate(perm, xsv_ext, ysv_ext, dx_ext, dy_ext)
            grad_y += -4 * dy_ext * attn_ext * open_extrapolate(perm, xsv_ext, ysv_ext, dx_ext, dy_ext)

    # Contribution from (0,0) or (1,1)
    attn0 = 2 - dx0 * dx0 - dy0 * dy0
    if attn0 > 0:
        attn0 *= attn0
        grad_x += -4 * dx0 * attn0 * open_extrapolate(perm, xsb, ysb, dx0, dy0)
        grad_y += -4 * dy0 * attn0 * open_extrapolate(perm, xsb, ysb, dx0, dy0)

    return np.array([grad_x / 47.0, grad_y / 47.0])


@njit(fastmath=True, cache=True)
def open_noise2_grad2(perm, x, y):
    # Skew the input space to determine the simplex cell
    stretch_offset = (x + y) * OPEN_STRETCH
    xs = x + stretch_offset
    ys = y + stretch_offset

    # Calculate the integer grid coords of simplex cell origin
    xsb = int(np.floor(xs))
    ysb = int(np.floor(ys))

    # Unskew the cell back to the original space
    squish_offset = (xsb + ysb) * OPEN_SQUASH
    xb = xsb + squish_offset
    yb = ysb + squish_offset

    # Determine simplex region we are in
    xins = xs - xsb
    yins = ys - ysb
    in_sum = xins + yins

    # Calculate distances from cell origin
    dx0 = x - xb
    dy0 = y - yb

    # Second derivatives initialization
    d2x, d2y = 0.0, 0.0  # Second derivative with respect to x and y

    # Contribution from (1,0)
    dx1 = dx0 - 1 - OPEN_SQUASH
    dy1 = dy0 - 0 - OPEN_SQUASH
    attn1 = 2 - dx1 * dx1 - dy1 * dy1
    if attn1 > 0:
        attn1 *= attn1
        d2x += -4 * (attn1 + dx1 * dx1 * -8 * attn1) * open_extrapolate(perm, xsb + 1, ysb + 0, dx1, dy1)
        d2y += -4 * (attn1 + dy1 * dy1 * -8 * attn1) * open_extrapolate(perm, xsb + 1, ysb + 0, dx1, dy1)

    # Contribution from (0,1)
    dx2 = dx0 - 0 - OPEN_SQUASH
    dy2 = dy0 - 1 - OPEN_SQUASH
    attn2 = 2 - dx2 * dx2 - dy2 * dy2
    if attn2 > 0:
        attn2 *= attn2
        d2x += -4 * (attn2 + dx2 * dx2 * -8 * attn2) * open_extrapolate(perm, xsb + 0, ysb + 1, dx2, dy2)
        d2y += -4 * (attn2 + dy2 * dy2 * -8 * attn2) * open_extrapolate(perm, xsb + 0, ysb + 1, dx2, dy2)

    # For inside the simplex at (0,0)
    if in_sum <= 1:
        zins = 1 - in_sum
        if zins > xins or zins > yins:  # (0,0) is one of the closest two triangular vertices
            if xins > yins:
                # (1,0) is the other closest vertex
                xsv_ext = xsb + 1
                ysv_ext = ysb - 1
                dx_ext = dx0 - 1
                dy_ext = dy0 + 1
            else:
                # (0,1) is the other closest vertex
                xsv_ext = xsb - 1
                ysv_ext = ysb + 1
                dx_ext = dx0 + 1
                dy_ext = dy0 - 1
        else:  # (1,0) and (0,1) are the closest two vertices
            xsv_ext = xsb + 1
            ysv_ext = ysb + 1
            dx_ext = dx0 - 1 - 2 * OPEN_SQUASH
            dy_ext = dy0 - 1 - 2 * OPEN_SQUASH
        # Second derivative contributions for this part
        attn_ext = 2 - dx_ext * dx_ext - dy_ext * dy_ext
        if attn_ext > 0:
            attn_ext *= attn_ext
            d2x += (
                -4
                * (attn_ext + dx_ext * dx_ext * -8 * attn_ext)
                * open_extrapolate(perm, xsv_ext, ysv_ext, dx_ext, dy_ext)
            )
            d2y += (
                -4
                * (attn_ext + dy_ext * dy_ext * -8 * attn_ext)
                * open_extrapolate(perm, xsv_ext, ysv_ext, dx_ext, dy_ext)
            )

    else:  # We're inside the triangle (2-Simplex) at (1,1)
        zins = 2 - in_sum
        if zins < xins or zins < yins:  # (0,0) is one of the closest two triangular vertices
            if xins > yins:
                # (1,0) is the other closest vertex
                xsv_ext = xsb + 2
                ysv_ext = ysb + 0
                dx_ext = dx0 - 2 - 2 * OPEN_SQUASH
                dy_ext = dy0 + 0 - 2 * OPEN_SQUASH
            else:
                # (0,1) is the other closest vertex
                xsv_ext = xsb + 0
                ysv_ext = ysb + 2
                dx_ext = dx0 + 0 - 2 * OPEN_SQUASH
                dy_ext = dy0 - 2 - 2 * OPEN_SQUASH
        else:  # (1,0) and (0,1) are the closest two vertices
            dx_ext = dx0
            dy_ext = dy0
            xsv_ext = xsb
            ysv_ext = ysb

        # Update the simplex coordinates for (1,1)
        xsb += 1
        ysb += 1
        dx0 = dx0 - 1 - 2 * OPEN_SQUASH
        dy0 = dy0 - 1 - 2 * OPEN_SQUASH

        # Second derivative contributions for this part
        attn_ext = 2 - dx_ext * dx_ext - dy_ext * dy_ext
        if attn_ext > 0:
            attn_ext *= attn_ext
            d2x += (
                -4
                * (attn_ext + dx_ext * dx_ext * -8 * attn_ext)
                * open_extrapolate(perm, xsv_ext, ysv_ext, dx_ext, dy_ext)
            )
            d2y += (
                -4
                * (attn_ext + dy_ext * dy_ext * -8 * attn_ext)
                * open_extrapolate(perm, xsv_ext, ysv_ext, dx_ext, dy_ext)
            )

    # Contribution from (0,0) or (1,1)
    attn0 = 2 - dx0 * dx0 - dy0 * dy0
    if attn0 > 0:
        attn0 *= attn0
        d2x += -4 * (attn0 + dx0 * dx0 * -8 * attn0) * open_extrapolate(perm, xsb, ysb, dx0, dy0)
        d2y += -4 * (attn0 + dy0 * dy0 * -8 * attn0) * open_extrapolate(perm, xsb, ysb, dx0, dy0)

    return np.array([d2x / 47.0, d2y / 47.0])


@njit(fastmath=True, parallel=True, cache=True)
def simplex_fractal_noise(
    perm, width, height, scale, octaves, persistence, lacunarity, x_offset=0, y_offset=0, start_frequency=1
):
    noise_map = np.zeros((height, width))
    for y in prange(height):
        for x in range(width):
            # Normalize coordinates for scale
            nx, ny = (x + x_offset) / scale, (y + y_offset) / scale

            noise_value = 0
            amplitude = 1
            frequency = start_frequency
            max_amplitude = 0

            # Generate fractal noise by summing multiple octaves
            for _ in range(octaves):
                noise_value += noise2(perm, nx * frequency, ny * frequency) * amplitude
                max_amplitude += amplitude
                amplitude *= persistence
                frequency *= lacunarity
            # Normalize the result to [-1, 1]
            noise_map[y, x] = noise_value / max_amplitude
    return noise_map


@njit(fastmath=True, parallel=True, cache=True)
def open_simplex_fractal_noise(
    perm, width, height, scale, octaves, persistence, lacunarity, x_offset=0, y_offset=0, start_frequency=1
):
    noise_map = np.zeros((height, width))
    for y in prange(height):
        for x in range(width):
            # Normalize coordinates for scale
            nx, ny = (x + x_offset) / scale, (y + y_offset) / scale

            noise_value = 0
            amplitude = 1
            frequency = start_frequency
            max_amplitude = 0

            # Generate fractal noise by summing multiple octaves
            for _ in range(octaves):
                noise_value += open_noise2(perm, nx * frequency, ny * frequency) * amplitude
                max_amplitude += amplitude
                amplitude *= persistence
                frequency *= lacunarity
            # Normalize the result to [-1, 1]
            noise_map[y, x] = noise_value / max_amplitude
    return noise_map


@njit(fastmath=True, cache=True)
def point_open_simplex_fractal_noise(
    perm, x, y, scale, octaves, persistence, lacunarity, x_offset=0, y_offset=0, start_frequency=1
):
    nx, ny = (x + x_offset) / scale, (y + y_offset) / scale
    noise_value = 0
    amplitude = 1
    frequency = start_frequency
    max_amplitude = 0

    for _ in range(octaves):
        noise_value += open_noise2(perm, nx * frequency, ny * frequency) * amplitude
        max_amplitude += amplitude
        amplitude *= persistence
        frequency *= lacunarity

    return noise_value / max_amplitude


@njit(fastmath=True, cache=True, parallel=True)
def batch_open_simplex_fractal_noise(
    perm, points, scale, octaves, persistence, lacunarity, x_offset=0, y_offset=0, start_frequency=1
):
    n = len(points)
    results = np.empty(n, dtype=np.float32)

    for i in prange(n):
        x, y = points[i]
        nx, ny = (x + x_offset) / scale, (y + y_offset) / scale
        noise_value = 0.0
        amplitude = 1.0
        frequency = start_frequency
        max_amplitude = 0.0

        for _ in range(octaves):
            noise_value += open_noise2(perm, nx * frequency, ny * frequency) * amplitude
            max_amplitude += amplitude
            amplitude *= persistence
            frequency *= lacunarity

        results[i] = noise_value / max_amplitude

    return results


@njit(fastmath=True, parallel=True, cache=True)
def warped_open_simplex_fractal_noise(
    perm,
    width,
    height,
    scale,
    octaves,
    persistence,
    lacunarity,
    warp_x=None,
    warp_y=None,
    warp_strength=0,
    x_offset=0,
    y_offset=0,
    start_frequency=1,
):
    noise_map = np.zeros((height, width))
    for y in prange(height):
        for x in range(width):
            # Normalize coordinates for scale
            nx, ny = (
                (x + x_offset + warp_x[y, x] * warp_strength) / scale,
                (y + y_offset + warp_y[y, x] * warp_strength) / scale,
            )

            noise_value = 0
            amplitude = 1
            frequency = start_frequency
            max_amplitude = 0

            # Generate fractal noise by summing multiple octaves
            for _ in range(octaves):
                noise_value += open_noise2(perm, nx * frequency, ny * frequency) * amplitude
                max_amplitude += amplitude
                amplitude *= persistence
                frequency *= lacunarity
            # Normalize the result to [-1, 1]
            noise_map[y, x] = noise_value / max_amplitude
    return noise_map


@njit(fastmath=True, parallel=True, cache=True)
def uber_noise(
    perm,
    width,
    height,
    scale,
    octaves,
    sharpness,
    feature_amp,
    slope_erosion,
    altitude_erosion,
    ridge_erosion,
    lacunarity,
    init_gain,
    x_offset=0,
    y_offset=0,
    start_frequency=1,
):
    ### https://www.sbgames.org/sbgames2018/files/papers/ComputacaoShort/188264.pdf
    noise_map = np.zeros((height, width))
    for y in prange(height):
        for x in range(width):
            nx, ny = (x + x_offset) / scale, (y + y_offset) / scale

            noise_value = 0
            # amp_sum = 0
            # amp, damp_amp = 1, 1
            amp = 1
            slope_dsum = np.array([0.0, 0.0])
            # ridge_dsum = np.array([0.0, 0.0])
            frequency = start_frequency
            gain = init_gain

            for _ in range(octaves):
                n = open_noise2(perm, nx * frequency, ny * frequency)
                grad = open_noise2_grad(perm, nx * frequency, ny * frequency)
                if ridge_erosion > 0:
                    grad2 = open_noise2_grad2(perm, nx * frequency, ny * frequency)
                    c = 0.5 * (grad2[0] + grad2[1])
                    delt = 1 / (1 + abs(min(0.0, c)))
                else:
                    delt = 1

                billow = abs(n)
                ridge = 1 - abs(n)

                if sharpness > 0:
                    n = n * (1 - sharpness) + billow * sharpness
                else:
                    n = n * (1 - abs(sharpness)) + ridge * abs(sharpness)

                # n = n*(1-sharpness) + ridge*sharpness
                n = n * feature_amp

                slope_dsum += grad * slope_erosion
                noise_value += amp * n / (1 + slope_dsum[0] ** 2 + slope_dsum[1] ** 2)

                frequency *= lacunarity
                amp *= gain * (1 - altitude_erosion) + gain * max(0, noise_value) * altitude_erosion

                gain = gain * (1 - ridge_erosion) + gain * delt * ridge_erosion

                # nx, ny = np.cos(theta)*nx - np.sin(theta)*ny, np.sin(theta)*nx + np.cos(theta)*ny

            noise_map[y, x] = noise_value
    return noise_map


@njit(fastmath=True, parallel=True, cache=True)
def warped_uber_noise(
    perm,
    width,
    height,
    scale,
    octaves,
    sharpness,
    feature_amp,
    slope_erosion,
    altitude_erosion,
    ridge_erosion,
    lacunarity,
    init_gain,
    warp_x=None,
    warp_y=None,
    warp_strength=0,
    x_offset=0,
    y_offset=0,
    start_frequency=1,
):
    ### https://www.sbgames.org/sbgames2018/files/papers/ComputacaoShort/188264.pdf
    noise_map = np.zeros((height, width))
    for y in prange(height):
        for x in range(width):
            nx = (x + x_offset + warp_x[y, x] * warp_strength) / scale
            ny = (y + y_offset + warp_y[y, x] * warp_strength) / scale

            noise_value = 0
            amp = 1
            slope_dsum = np.array([0.0, 0.0])
            frequency = start_frequency
            gain = init_gain

            theta = 0

            for _ in range(octaves):
                n = open_noise2(perm, nx * frequency, ny * frequency)
                grad = open_noise2_grad(perm, nx * frequency, ny * frequency)
                grad2 = open_noise2_grad2(perm, nx * frequency, ny * frequency)
                c = 0.5 * (grad2[0] + grad2[1])

                billow = np.abs(n)
                ridge = 1 - np.abs(n)

                if sharpness > 0:
                    n = n * (1 - sharpness) + billow * sharpness
                else:
                    n = n * (1 - abs(sharpness)) + ridge * abs(sharpness)

                # n = n*(1-sharpness) + ridge*sharpness
                n = n * feature_amp

                slope_dsum += grad * slope_erosion
                noise_value += amp * n / (1 + slope_dsum[0] ** 2 + slope_dsum[1] ** 2)

                frequency *= lacunarity
                amp *= gain * (1 - altitude_erosion) + gain * max(0, noise_value) * altitude_erosion

                gain = gain * (1 - ridge_erosion) + gain * (1 / (1 + abs(min(0.0, c)))) * ridge_erosion

                nx, ny = np.cos(theta) * nx - np.sin(theta) * ny, np.sin(theta) * nx + np.cos(theta) * ny

            noise_map[y, x] = noise_value
    return noise_map


def snoise_fractal_noise(
    width, height, scale, octaves, persistence, lacunarity, x_offset=0, y_offset=0, start_frequency=1
):
    noise_map = np.zeros((height, width))
    for y in range(height):
        for x in range(width):
            # Normalize coordinates for scale with offsets
            nx, ny = (x + x_offset) / scale, (y + y_offset) / scale

            noise_value = 0
            amplitude = 1
            frequency = start_frequency
            max_amplitude = 0

            # Generate fractal noise by summing multiple octaves
            for _ in range(octaves):
                noise_value += snoise2(nx * frequency, ny * frequency) * amplitude
                max_amplitude += amplitude
                amplitude *= persistence
                frequency *= lacunarity

            # Normalize the result to [-1, 1]
            noise_map[y, x] = noise_value / max_amplitude

    return noise_map
