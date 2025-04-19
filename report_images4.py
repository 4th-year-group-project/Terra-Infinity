import numpy as np
import matplotlib.pyplot as plt
from generation import Display, Noise, tools

noise_gen = Noise(seed=0, width=1024, height=1024)
phasor = noise_gen.phasor_noise(anisotropy=0.1, freq_range=(5,7), num_phasors=10, direction_bias=-np.pi/4)

simp = noise_gen.fractal_simplex_noise(
    noise="open", x_offset=0, y_offset=0, scale=256, octaves=3, persistence=0.45, lacunarity=1.7
)   
# signal_phase = (phasor + 1) / 2.0
# # Apply PWM profile: 1 if within duty cycle, 0 otherwise
# noise = (signal_phase < 0.3).astype(float)

plt.imshow(phasor > 0, cmap='gray', origin='lower')
plt.axis('off')
#plt.show()
plt.savefig("phasor_noise_f=5,7.png", bbox_inches="tight", pad_inches=0, dpi=300)

# heightmap = tools.smooth_min(tools.smooth_max(phasor, 0, 1), 0.3, 0.3)

# heightmap = tools.normalize(simp, 0, 0.5) + tools.normalize(phasor, 0, 0.06)

# display = Display(heightmap, height_scale=250, colormap='hot_desert')
# display.display_heightmap()
# display.save_heightmap("phasor_noise_3d_w_noise.png")