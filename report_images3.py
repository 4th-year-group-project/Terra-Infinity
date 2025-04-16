import numpy as np
import matplotlib.pyplot as plt
from generation import Display, Noise, tools

noise_gen = Noise(seed=0, width=1024, height=1024)
worley = noise_gen.worley_noise(density=1024, k=1, p=1, distribution="poisson", radius=150)

plt.imshow(worley, cmap='gray', origin='lower')
plt.axis('off')
plt.show()
#plt.savefig("worley_noise_p=1.png", bbox_inches='tight', pad_inches=0, dpi=300)