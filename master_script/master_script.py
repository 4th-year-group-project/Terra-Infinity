from biomes.create_voronoi import get_chunk_polygons
from cellular_automata.voronoi import terrain_voronoi
import matplotlib.pyplot as plt

def fetch_superchunk_data(coords, seed):
    relevant_polygons_edges, relevant_polygons_points = get_chunk_polygons(coords, seed)

    # Next step: Land water split
    #            |
    #            |                     midpoints displacing coastal edges can go here
    #            v            
    # Next step: Biome assignment
    #            |
    #            |                     or here
    #            v
    # Next step: Heightmap generation

    #Based on biome information & seed & such, choose methods to generate heightmap
    #Right now just CA heightmap

    # e.g.
    # voronoi_id, gen_method = determine_heightmap_method(relevant_polygons_edges, relevant_polygons_points, seed)

    superchunk_heightmap = terrain_voronoi(relevant_polygons_edges, relevant_polygons_points)
    # print(superchunk_heightmap.shape)
    # plt.figure()
    # plt.imshow(superchunk_heightmap, cmap='gray')
    # plt.axis('off')
    # plt.show()
    return superchunk_heightmap

if __name__ == "__main__":
    seed = 710
    coords = (0,0)
    fetch_superchunk_data(coords, seed)