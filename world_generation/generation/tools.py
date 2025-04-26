import numpy as np


def normalize(array, a=0, b=1):
    return a + ((array - array.min()) / (array.max() - array.min())) * (b - a)


def blend(array1, array2, alpha=0.5):
    return alpha * array1 + (1 - alpha) * array2


def sawtooth(array):
    return array - np.floor(array)


def smooth(x, a=10, b=0.5):
    return 1 / (1 + np.exp(-a * (x - b)))


def low_smooth(x, a=10, b=0.5):
    return x / (1 + np.exp(-a * (x - b)))


def high_smooth(x, a=10, b=0.5):
    return x + ((1 - x) / (1 + np.exp(-a * (x - b))))


def smooth_min(a, b, k):
    h = np.clip((b - a + k) / (2 * k), 0, 1)
    return a * h + b * (1 - h) - k * h * (1 - h)


def smooth_max(a, b, k):
    k = -k
    h = np.clip((b - a + k) / (2 * k), 0, 1)
    return a * h + b * (1 - h) - k * h * (1 - h)


def map01(x, a, b):
    return a + (b - a) * x


def map0100(value, a, b):
    return a + (value / 100) * (b - a)


### Safe domain warping:
# warp power = w
# buffer = floor(w/2) + floor(3*w/2)
# input target, warpx and warpy with dimensions + buffer
# slice target with:
# target = target[buffer//2:-buffer//2, buffer//2:-buffer//2]


def domain_warp(target, warp_x, warp_y, warp_strength=100):
    warped_target = np.zeros_like(target)
    warp_x = (warp_x * warp_strength - (warp_strength / 2)).astype(int)
    warp_y = (warp_y * warp_strength - (warp_strength / 2)).astype(int)
    x_coords, y_coords = np.meshgrid(np.arange(target.shape[1]), np.arange(target.shape[0]))
    new_x = np.clip(x_coords + warp_x, 0, target.shape[1] - 1)
    new_y = np.clip(y_coords + warp_y, 0, target.shape[0] - 1)
    warped_target = target[new_y, new_x]
    return warped_target
