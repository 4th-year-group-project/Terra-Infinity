from biomes.create_voronoi import get_chunk_polygons
from cellular_automata.voronoi import terrain_voronoi

def fetch_superchunk_data(coords, seed):
    relevant_polygons_edges, relevant_polygons_points = get_chunk_polygons(coords, seed)

    # Next step: Land water split
    #            |
    #            v
    # Next step: Biome assignment
    #            |
    #            v
    # Next step: Heightmap generation

    #Right now just CA heightmaps
    superchunk_heightmap = terrain_voronoi(relevant_polygons_edges, relevant_polygons_points)
    return superchunk_heightmap

if __name__ == "__main__":
    seed = 710
    coords = (0,0)
    fetch_superchunk_data(coords, seed)