from biomes.create_voronoi import get_chunk_polygons
from biomes.land_water_map import determine_landmass
from biomes.climate_map import determine_biomes
from cellular_automata.voronoi import terrain_voronoi
import matplotlib.pyplot as plt

def fetch_superchunk_data(coords, seed):
    relevant_polygons_edges, relevant_polygons_points = get_chunk_polygons(coords, seed)

    land_polygon_edges, land_polygon_points = determine_landmass(relevant_polygons_edges, relevant_polygons_points, seed)

    # Next step: Land water split
    # landmass_classifications = determine_landmass(relevant_polygons_edges, relevant_polygons_points, seed)
    #            |
    #            |                     midpoints displacing coastal edges can go here
    #            v            
    # Next step: Biome assignment
    # biomes = determine_biome(relevant_polygons_edges, relevant_polygons_points, landmass_classifications, seed)
    #            |
    #            |                     or here
    #            v
    # Next step: Heightmap generation

    #Based on biome information & seed & such, choose methods to generate heightmap
    #Right now just CA heightmap

    # e.g.
    # voronoi_id, gen_method = determine_heightmap_method(relevant_polygons_edges, relevant_polygons_points, seed)

    superchunk_heightmap, reconstructed_image = terrain_voronoi(land_polygon_edges, land_polygon_points)
    print(superchunk_heightmap.shape)
    plt.figure(figsize=(10, 5))
    plt.subplot(1, 2, 1)
    plt.imshow(superchunk_heightmap, cmap='gray')
    plt.axis('off')
    plt.subplot(1, 2, 2)
    plt.imshow(reconstructed_image, cmap='gray')
    plt.show()
    return superchunk_heightmap

if __name__ == "__main__":
    seed = 5
    coords = (0,0)
    fetch_superchunk_data(coords, seed)