import numpy as np
from geom import *
from fractal_coastline import *
from poisson_sample import PoissonDisc

shapes = {}
shapes["triangle"] = Polygon([Point([0.5,-np.sqrt(3)/4]), Point([0,np.sqrt(3)/4]),Point([-0.5,-np.sqrt(3)/4])])
shapes["hexagon"] = Polygon([Point([1,0]),Point([0.5,np.sqrt(3)/2]),Point([-0.5,np.sqrt(3)/2]),Point([-1,0]),Point([-0.5,-np.sqrt(3)/2]),Point([0.5,-np.sqrt(3)/2])])
shapes["square"] = Polygon([Point([1,-1]),Point([1,1]),Point([-1,1]),Point([-1,-1])])
shapes["rectangle"] = Polygon([Point([-3, -0.5]),Point([-2, -0.5]),Point([-1, -0.5]), Point([0, -0.5]),Point([1, -0.5]),Point([2, -0.5]),Point([3, -0.5]),
                            Point([3, 0.5]),Point([2, 0.5]),Point([1, 0.5]),Point([0, 0.5]),Point([-1, 0.5]),Point([-2, 0.5]),Point([-3, 0.5])])
shapes["long"] = Polygon([Point([-3, -0.5]),Point([3, -0.5]),
                        Point([3, 0.5]),Point([-3, 0.5])])


def skewed_random(a, b, scale=1):
    exp_value = np.random.exponential(scale)
    result = a + (b - a) * (1 - np.exp(-exp_value / scale))  # Adjust to [a, b]
    return min(result, b)  # Ensure it doesn't exceed b

dis = PoissonDisc(width=200, height=100, r=15*(1-0.5**10)/(1-0.5), k=5, max_points=30)
points = dis.sample()

islands = []
for point in points:
    polygon = Polygon([Point([1,-1]),Point([1,1]),Point([-1,1]),Point([-1,-1])])
    polygon.translate(point)
    polygon.scale_from_center(skewed_random(5,20))
    coastline = Coastline(shape=polygon,
                        iterations=10,
                        displacement=polygon.calculate_distances()[1]/2, 
                        width=0.34,
                        roughness=0.5,
                        display=False
                        )
    coastline.fractal()
    islands = islands + coastline.polygons

plot = Plot()
plot.display(islands)
plt.show()
