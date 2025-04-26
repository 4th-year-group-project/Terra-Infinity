import matplotlib.colors as mcolors
import matplotlib.pyplot as plt
import numpy as np
import vedo
from PIL import Image


class Display:
    def __init__(self, height_array, height_scale=250, colormap="terrain"):
        self.height_array = np.array(height_array) * height_scale  # Scale height between 0 and `height_scale`
        self.height_scale = height_scale
        self.colormap = colormap
        self.rows, self.cols = self.height_array.shape
        self.x, self.y = np.meshgrid(np.arange(self.cols), np.arange(self.rows))
        self.z = self.height_array  # The height values
        self.vertices = np.c_[self.x.ravel(), self.y.ravel(), self.z.ravel()]
        self.faces = self._generate_faces()

    def _generate_faces(self):
        faces = []
        for i in range(self.rows - 1):
            for j in range(self.cols - 1):
                p1 = i * self.cols + j
                p2 = p1 + 1
                p3 = p1 + self.cols
                p4 = p3 + 1
                faces.append([p1, p2, p4])
                faces.append([p1, p4, p3])
        return faces

    def _get_colormap(self):
        colormaps = {
            "terrain": plt.get_cmap("terrain"),
            "gray": plt.get_cmap("gray"),
            "sand": self._create_colormap([(0.9, 0.8, 0.6), (0.95, 0.85, 0.65), (1, 0.9, 0.7), (0.8, 0.7, 0.5)]),
            "grass": self._create_colormap([(0.2, 0.4, 0.1), (0.3, 0.5, 0.2), (0.4, 0.6, 0.3), (0.1, 0.3, 0.05)]),
            "volcanic": self._create_colormap([(0.8, 0.3, 0.1), (0.7, 0.2, 0.1), (0.5, 0.15, 0.1), (0.3, 0.2, 0.2), (0.2, 0.2, 0.2), (0.5, 0.5, 0.5)]),
            "badlands": self._create_colormap([(0.95, 0.8, 0.5), (0.85, 0.6, 0.3), (0.75, 0.5, 0.2), (0.6, 0.3, 0.1), (0.5, 0.25, 0.1), (0.4, 0.2, 0.05),  (0.3, 0.15, 0.05), (0.2, 0.1, 0.05)]),
            "hot_desert": self._create_colormap([(1.0, 0.9, 0.7), (0.95, 0.8, 0.6), (0.9, 0.7, 0.5), (0.8, 0.6, 0.4),(0.7, 0.5, 0.3),(0.6, 0.4, 0.2),(0.5, 0.3, 0.1)]),
            "rocky_muddy": self._create_colormap([(0.2, 0.15, 0.1), (0.3, 0.2, 0.15),  (0.4, 0.3, 0.2), (0.5, 0.4, 0.3),  (0.65, 0.5, 0.35), (0.75, 0.6, 0.45)]),
            "cliffs": self._create_colormap([(0.0, 0.1, 0.4),(0.0, 0.5, 0.7), (0.8, 0.7, 0.5),  (0.6, 0.6, 0.6), (0.5, 0.5, 0.5), (0.2, 0.5, 0.2), (0.3, 0.6, 0.3)]),
            "ocean": self._create_colormap([(0.0, 0.05, 0.2), (0.0, 0.15, 0.35),(0.0, 0.25, 0.5),(0.0, 0.4, 0.55),  (0.0, 0.5, 0.6),  (0.2, 0.6, 0.65)]),
            "mesa": self._create_colormap([(0.45, 0.2, 0.15), (0.65, 0.35, 0.2), (0.85, 0.6, 0.3),   (0.9, 0.8, 0.6),  (0.75, 0.4, 0.25), (0.35, 0.25, 0.15)], n=24),
            "winter": self._create_colormap([(0.3, 0.5, 0.8),(0.6, 0.7, 0.9), (1.0, 1.0, 1.0), (0.75, 0.75, 0.8), (0.6, 0.6, 0.7), (0.9, 0.9, 1.0),(1.0, 1.0, 1.0)]),
            "beach": self._create_colormap([(0.1, 0.3, 0.6),  (0.2, 0.5, 0.8),(0.3, 0.75, 0.9), (0.4, 0.8, 0.95), (0.9, 0.8, 0.6), (0.95, 0.85, 0.65),(1.0, 0.9, 0.7)]),
            "dusty": self._create_colormap([(0.6, 0.35, 0.1), (0.7, 0.45, 0.15), (0.9, 0.55, 0.2), (0.95, 0.65, 0.3), (0.95, 0.7, 0.35)]),
            "ashen": self._create_colormap([(0,0,0), (0.1, 0.1, 0.1), (0.3, 0.3, 0.3), (0.5, 0.5, 0.5), (0.7, 0.7, 0.7), (0.9, 0.9, 0.9)]),
            "lush_plains": self._create_colormap([(0.0, 0.1, 0.4), (0.6, 0.8, 0.9), (0.4, 0.7, 0.4), (0.4, 0.7, 0.4), (0.3, 0.6, 0.3), (0.2, 0.5, 0.2)]),
            "lush_mountain": self._create_colormap([(0.5, 0.8, 0.4), (0.3, 0.6, 0.2), (0.5, 0.5, 0.5), (0.5, 0.5, 0.5), (0.5, 0.5, 0.5), (0.7, 0.7, 0.7), (0.7, 0.7, 0.7), (1.0, 1.0, 1.0), (1.0, 1.0, 1.0)]),
            "rocky_field": self._create_colormap([(0.4, 0.7, 0.3), (0.4, 0.7, 0.3), (0.3, 0.6, 0.2), (0.6, 0.6, 0.6), (0.8, 0.8, 0.8), (0.9, 0.9, 0.9)]),
            "lush_grass": self._create_colormap([(0.2, 0.6, 0.2), (0.3, 0.7, 0.3), (0.4, 0.8, 0.4), (0.5, 0.9, 0.5), (0.6, 0.9, 0.6)]),
            "jungle": self._create_colormap([(0.05, 0.15, 0.05), (0.1, 0.3, 0.1), (0.2, 0.4, 0.15), (0.1, 0.5, 0.2), (0.15, 0.6, 0.3), (0.3, 0.7, 0.4), (0.4, 0.5, 0.2)]),
            "oasis": self._create_colormap([(0.0, 0.2, 0.4), (0.0, 0.2, 0.4), (0.0, 0.4, 0.6), (0.0, 0.4, 0.6), (0.0, 0.4, 0.6), (0.9, 0.8, 0.6), (0.95, 0.85, 0.65), (0.95, 0.85, 0.65), (0.95, 0.85, 0.65), (0.6, 0.7, 0.4), (0.3, 0.6, 0.3)]),
            "moon": self._create_colormap([(0.1, 0.1, 0.1), (0.3, 0.3, 0.3), (0.5, 0.5, 0.5), (0.7, 0.7, 0.7), (0.9, 0.9, 0.9), (1.0, 1.0, 1.0)]),
            "bog": self._create_colormap([(0.2, 0.15, 0.05), (0.25, 0.2, 0.1), (0.3, 0.25, 0.15), (0.35, 0.3, 0.2), (0.4, 0.4, 0.3), (0.5, 0.6, 0.4), (0.3, 0.5, 0.2), (0.3, 0.5, 0.2), (0.3, 0.5, 0.2), (0.2, 0.4, 0.1)]),
        }
        return colormaps.get(self.colormap, plt.get_cmap("terrain"))

    def _create_colormap(self, colors, n=256):
        return mcolors.LinearSegmentedColormap.from_list(self.colormap, colors, N=n)

    def gen_terrain_mesh(self):
        faces = self.faces
        terrain_mesh = vedo.Mesh([self.vertices, faces])
        colormap = self._get_colormap()
        terrain_mesh.cmap(colormap, self.z.ravel())
        return terrain_mesh

    def display_heightmap(self):
        terrain_mesh = self.gen_terrain_mesh()
        plotter = vedo.Plotter()
        plotter.show(terrain_mesh, "3D Heightmap Terrain", axes=0, viewup="z", zoom=True)

    def stitch(self, image1, image2, name="combined_image.jpg"):
        whitespace = 50
        new_width = image1.width + image2.width + whitespace
        new_height = max(image1.height, image2.height)
        new_image = Image.new("RGB", (new_width, new_height), (255, 255, 255))
        new_image.paste(image1, (0, 0))
        new_image.paste(image2, (image1.width + whitespace, 0))
        new_image = new_image.resize((new_image.width // 2, new_image.height // 2))

        new_image.save(name)

    def save_heightmap(self, name="heightmap"):
        terrain_mesh = self.gen_terrain_mesh()

        plotter = vedo.Plotter()
        plotter.show(terrain_mesh, "", viewup="z", zoom=True, interactive=False)
        plotter.camera.Zoom(1.6)
        plotter.look_at()
        plotter.render()
        image1 = plotter.screenshot(asarray=True)
        image1 = Image.fromarray(image1)

        plotter.camera.Elevation(315)
        plotter.reset_camera()
        plotter.camera.Zoom(1.4)
        plotter.render()
        image2 = plotter.screenshot(asarray=True)
        image2 = Image.fromarray(image2)

        self.stitch(image1, image2, name)
