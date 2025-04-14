#include "Parameters.hpp"
#include <cstdlib>
#include <filesystem>
#include <nlohmann/json.hpp>

#include <chrono>

using json = nlohmann::json;
using namespace std;
namespace fs = std::filesystem;

// Function to reset the parameters to their default values
void Parameters::setDefaultValues() {
    *this = Parameters();
    setRandomSeed();
}

// Default constructor for parameters, initializes all parameters to default values
Parameters::Parameters() {
    // Global parameters
    seed = 12345;
    maxHeight = 100;
    oceanCoverage = 50;
    continentSize = 50;
    roughness = 50;
    mountainousness = 50;
    coastlineRoughness = 50;
    biomeSize = 50;
    warmth = 50;
    wetness = 50;
    riverFrequency = 50;
    riverWidth = 5;
    riverDepth = 5;
    riverMeandering = 50;
    riverSmoothness = 50;
    treesDensity = 50;

    // Boreal forest
    borealForestSelected = true;
    borealForestPlainsMaxHeight = 50;
    borealForestPlainsOccurrenceProbability = 50;
    borealForestPlainsEvenness = 50;
    borealForestPlainsTreeDensity = 50;
    borealForestHillsMaxHeight = 50;
    borealForestHillsOccurrenceProbability = 50;
    borealForestHillsBumpiness = 50;
    borealForestHillsTreeDensity = 50;
    borealForestMountainsMaxHeight = 50;
    borealForestMountainsOccurrenceProbability = 50;
    borealForestMountainsRuggedness = 50;
    borealForestMountainsTreeDensity = 50;

    // Grassland
    grasslandSelected = true;
    grasslandPlainsMaxHeight = 73;
    grasslandPlainsOccurrenceProbability = 57;
    grasslandPlainsEvenness = 22;
    grasslandPlainsTreeDensity = 13;
    grasslandHillsMaxHeight = 64;
    grasslandHillsOccurrenceProbability = 11;
    grasslandHillsBumpiness = 94;
    grasslandHillsTreeDensity = 35;
    grasslandRockyFieldsMaxHeight = 59;
    grasslandRockyFieldsOccurrenceProbability = 88;
    grasslandRockyFieldsRockiness = 72;
    grasslandRockyFieldsTreeDensity = 90;
    grasslandTerracedFieldsMaxHeight = 61;
    grasslandTerracedFieldsOccurrenceProbability = 10;
    grasslandTerracedFieldsSize = 56;
    grasslandTerracedFieldsTreeDensity = 44;
    grasslandTerracedFieldsSmoothness = 15;
    grasslandTerracedFieldsNumberOfTerraces = 19;

    // Tundra
    tundraSelected = true;
    tundraPlainsMaxHeight = 39;
    tundraPlainsOccurrenceProbability = 87;
    tundraPlainsEvenness = 1;
    tundraPlainsTreeDensity = 34;
    tundraBluntMountainsMaxHeight = 88;
    tundraBluntMountainsOccurrenceProbability = 25;
    tundraBluntMountainsRuggedness = 16;
    tundraBluntMountainsTreeDensity = 2;
    tundraPointyMountainsMaxHeight = 89;
    tundraPointyMountainsOccurrenceProbability = 67;
    tundraPointyMountainsSteepness = 10;
    tundraPointyMountainsFrequency = 97;
    tundraPointyMountainsTreeDensity = 59;

    // Savanna
    savannaSelected = true;
    savannaPlainsMaxHeight = 60;
    savannaPlainsOccurrenceProbability = 39;
    savannaPlainsEvenness = 23;
    savannaPlainsTreeDensity = 74;
    savannaMountainsMaxHeight = 42;
    savannaMountainsOccurrenceProbability = 19;
    savannaMountainsRuggedness = 99;
    savannaMountainsTreeDensity = 6;

    // Woodland
    woodlandSelected = true;
    woodlandHillsMaxHeight = 76;
    woodlandHillsOccurrenceProbability = 31;
    woodlandHillsBumpiness = 12;
    woodlandHillsTreeDensity = 65;

    // Tropical Rainforest
    tropicalRainforestSelected = true;
    tropicalRainforestPlainsMaxHeight = 44;
    tropicalRainforestPlainsOccurrenceProbability = 24;
    tropicalRainforestPlainsEvenness = 66;
    tropicalRainforestPlainsTreeDensity = 4;
    tropicalRainforestMountainsMaxHeight = 49;
    tropicalRainforestMountainsOccurrenceProbability = 29;
    tropicalRainforestMountainsRuggedness = 83;
    tropicalRainforestMountainsTreeDensity = 55;
    tropicalRainforestHillsMaxHeight = 61;
    tropicalRainforestHillsOccurrenceProbability = 76;
    tropicalRainforestHillsBumpiness = 90;
    tropicalRainforestHillsTreeDensity = 37;
    tropicalRainforestVolcanoesMaxHeight = 18;
    tropicalRainforestVolcanoesOccurrenceProbability = 26;
    tropicalRainforestVolcanoesSize = 86;
    tropicalRainforestVolcanoesTreeDensity = 42;
    tropicalRainforestVolcanoesThickness = 51;
    tropicalRainforestVolcanoesDensity = 20;

    // Temperate Rainforest
    temperateRainforestSelected = true;
    temperateRainforestHillsMaxHeight = 66;
    temperateRainforestHillsOccurrenceProbability = 12;
    temperateRainforestHillsBumpiness = 30;
    temperateRainforestHillsTreeDensity = 58;
    temperateRainforestMountainsMaxHeight = 96;
    temperateRainforestMountainsOccurrenceProbability = 37;
    temperateRainforestMountainsRuggedness = 82;
    temperateRainforestMountainsTreeDensity = 91;
    temperateRainforestSwampMaxHeight = 9;
    temperateRainforestSwampOccurrenceProbability = 88;
    temperateRainforestSwampWetness = 100;
    temperateRainforestSwampTreeDensity = 53;

    // Temperate Seasonal Forest
    temperateSeasonalForestSelected = true;
    temperateSeasonalForestHillsMaxHeight = 68;
    temperateSeasonalForestHillsOccurrenceProbability = 61;
    temperateSeasonalForestHillsBumpiness = 75;
    temperateSeasonalForestHillsTreeDensity = 80;
    temperateSeasonalForestHillsAutumnalOccurrence = 96;
    temperateSeasonalForestMountainsMaxHeight = 58;
    temperateSeasonalForestMountainsOccurrenceProbability = 8;
    temperateSeasonalForestMountainsRuggedness = 27;
    temperateSeasonalForestMountainsTreeDensity = 36;
    temperateSeasonalForestMountainsAutumnalOccurrence = 22;

    // Subtropical Desert
    subtropicalDesertSelected = true;
    subtropicalDesertDunesMaxHeight = 30;
    subtropicalDesertDunesOccurrenceProbability = 20;
    subtropicalDesertDunesSize = 62;
    subtropicalDesertDunesTreeDensity = 12;
    subtropicalDesertDunesDuneFrequency = 87;
    subtropicalDesertDunesDuneWaviness = 23;
    subtropicalDesertDunesBumpiness = 42;
    subtropicalDesertMesasMaxHeight = 61;
    subtropicalDesertMesasOccurrenceProbability = 7;
    subtropicalDesertMesasSize = 52;
    subtropicalDesertMesasTreeDensity = 15;
    subtropicalDesertMesasNumberOfTerraces = 41;
    subtropicalDesertMesasSteepness = 94;
    subtropicalDesertRavinesMaxHeight = 59;
    subtropicalDesertRavinesOccurrenceProbability = 21;
    subtropicalDesertRavinesDensity = 68;
    subtropicalDesertRavinesTreeDensity = 80;
    subtropicalDesertRavinesRavineWidth = 50;
    subtropicalDesertRavinesSmoothness = 11;
    subtropicalDesertRavinesSteepness = 27;
    subtropicalDesertOasisMaxHeight = 42;
    subtropicalDesertOasisOccurrenceProbability = 73;
    subtropicalDesertOasisSize = 84;
    subtropicalDesertOasisFlatness = 67;
    subtropicalDesertOasisTreeDensity = 3;
    subtropicalDesertOasisDuneFrequency = 79;
    subtropicalDesertCrackedMaxHeight = 62;
    subtropicalDesertCrackedOccurrenceProbability = 31;
    subtropicalDesertCrackedSize = 90;
    subtropicalDesertCrackedFlatness = 58;
    subtropicalDesertCrackedTreeDensity = 70;

    // Ocean
    oceanFlatSeabedMaxHeight = 46;
    oceanFlatSeabedEvenness = 36;
    oceanFlatSeabedOccurrenceProbability = 54;
    oceanVolcanicIslandsMaxHeight = 64;
    oceanVolcanicIslandsOccurrenceProbability = 85;
    oceanVolcanicIslandsSize = 22;
    oceanVolcanicIslandsThickness = 14;
    oceanVolcanicIslandsDensity = 31;
    oceanWaterStacksMaxHeight = 16;
    oceanWaterStacksOccurrenceProbability = 91;
    oceanWaterStacksSize = 33;
    oceanTrenchesDensity = 57;
    oceanTrenchesOccurrenceProbability = 26;
    oceanTrenchesTrenchWidth = 93;
    oceanTrenchesSmoothness = 97;

    // Textures
    borealTextureLow = "coast_sand_01_1k";
    borealTextureMidFlat = "rocky_terrain_02_1k";
    borealTextureMidSteep = "rock_05_1k";
    borealTextureHigh = "snow_02_1k";

    grassyTextureLow = "coast_sand_01_1k";
    grassyTextureMidFlat = "rocky_terrain_02_1k";
    grassyTextureMidSteep = "rock_05_1k";
    grassyTextureHigh = "snow_02_1k";

    grassyStoneTextureLow = "coast_sand_01_1k";
    grassyStoneTextureMidFlat = "rocky_terrain_02_1k";
    grassyStoneTextureMidSteep = "rock_05_1k";
    grassyStoneTextureHigh = "snow_02_1k";

    snowyTextureLow = "coast_sand_01_1k";
    snowyTextureMidFlat = "rocky_terrain_02_1k";
    snowyTextureMidSteep = "rock_05_1k";
    snowyTextureHigh = "snow_02_1k";

    icyTextureLow = "coast_sand_01_1k";
    icyTextureMidFlat = "rocky_terrain_02_1k";
    icyTextureMidSteep = "rock_05_1k";
    icyTextureHigh = "snow_02_1k";

    savannaTextureLow = "coast_sand_01_1k";
    savannaTextureMidFlat = "rocky_terrain_02_1k";
    savannaTextureMidSteep = "rock_05_1k";
    savannaTextureHigh = "snow_02_1k";

    woodlandTextureLow = "coast_sand_01_1k";
    woodlandTextureMidFlat = "rocky_terrain_02_1k";
    woodlandTextureMidSteep = "rock_05_1k";
    woodlandTextureHigh = "snow_02_1k";
    
    jungleTextureLow = "coast_sand_01_1k";
    jungleTextureMidFlat = "rocky_terrain_02_1k";
    jungleTextureMidSteep = "rock_05_1k";
    jungleTextureHigh = "snow_02_1k";

    jungleMountainsTextureLow = "coast_sand_01_1k";
    jungleMountainsTextureMidFlat = "rocky_terrain_02_1k";
    jungleMountainsTextureMidSteep = "rock_05_1k";
    jungleMountainsTextureHigh = "snow_02_1k";

    volcanicTextureLow = "coast_sand_01_1k";
    volcanicTextureMidFlat = "rocky_terrain_02_1k";
    volcanicTextureMidSteep = "rock_05_1k";
    volcanicTextureHigh = "snow_02_1k";

    temperateTextureLow = "coast_sand_01_1k";
    temperateTextureMidFlat = "rocky_terrain_02_1k";
    temperateTextureMidSteep = "rock_05_1k";
    temperateTextureHigh = "snow_02_1k";

    swampTextureLow = "coast_sand_01_1k";
    swampTextureMidFlat = "rocky_terrain_02_1k";
    swampTextureMidSteep = "rock_05_1k";
    swampTextureHigh = "snow_02_1k";

    seasonalForestTextureLow = "coast_sand_01_1k";
    seasonalForestTextureMidFlat = "rocky_terrain_02_1k";
    seasonalForestTextureMidSteep = "rock_05_1k";
    seasonalForestTextureHigh = "snow_02_1k";

    autumnTextureLow = "coast_sand_01_1k";
    autumnTextureMidFlat = "rocky_terrain_02_1k";
    autumnTextureMidSteep = "rock_05_1k";
    autumnTextureHigh = "snow_02_1k";

    mesaTextureLow = "coast_sand_01_1k";
    mesaTextureMidFlat = "rocky_terrain_02_1k";
    mesaTextureMidSteep = "rock_05_1k";
    mesaTextureHigh = "snow_02_1k";

    hotDesertTextureLow = "coast_sand_01_1k";
    hotDesertTextureMidFlat = "rocky_terrain_02_1k";
    hotDesertTextureMidSteep = "rock_05_1k";
    hotDesertTextureHigh = "snow_02_1k";

    dustyTextureLow = "coast_sand_01_1k";
    dustyTextureMidFlat = "rocky_terrain_02_1k";
    dustyTextureMidSteep = "rock_05_1k";
    dustyTextureHigh = "snow_02_1k";

    badlandsTextureLow = "coast_sand_01_1k";
    badlandsTextureMidFlat = "rocky_terrain_02_1k";
    badlandsTextureMidSteep = "rock_05_1k";
    badlandsTextureHigh = "snow_02_1k";

    oasisTextureLow = "coast_sand_01_1k";
    oasisTextureMidFlat = "rocky_terrain_02_1k";
    oasisTextureMidSteep = "rock_05_1k";
    oasisTextureHigh = "snow_02_1k";

    oceanTextureLow = "coast_sand_01_1k";
    oceanTextureMidFlat = "rocky_terrain_02_1k";
    oceanTextureMidSteep = "rock_05_1k";
    oceanTextureHigh = "snow_02_1k";

    cliffsTextureLow = "coast_sand_01_1k";
    cliffsTextureMidFlat = "rocky_terrain_02_1k";
    cliffsTextureMidSteep = "rock_05_1k";
    cliffsTextureHigh = "snow_02_1k";
}

// Function to set the random seed
void Parameters::setRandomSeed(){
    // Get the current time without using time function and initialise srand
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
    srand(millis);
    int msbRandom = rand();
    int lsbRandom = rand();
    uint64_t u_seed = (static_cast<uint64_t>(msbRandom) << 32) | static_cast<uint64_t>(lsbRandom);
    seed = static_cast<long>(u_seed);
}

// Function to save parameters to a JSON file
bool Parameters::saveToFile(string fileName, char filePathDelimitter) {
    nlohmann::json jsonData = {
        {"seed", seed},
        {"global_max_height", maxHeight},
        {"ocean_coverage", oceanCoverage},
        {"continent_size", continentSize},
        {"roughness", roughness},
        {"mountainousness", mountainousness},
        {"coastline_roughness", coastlineRoughness},
        {"biome_size", biomeSize},
        {"warmth", warmth},
        {"wetness", wetness},
        {"river_frequency", riverFrequency},
        {"river_width", riverWidth},
        {"river_depth", riverDepth},
        {"river_meandering", riverMeandering},
        {"river_smoothness", riverSmoothness},
        {"trees_density", treesDensity},
    
        {"boreal_forest", {
            {"selected", borealForestSelected},
            {"plains", {
                {"max_height", borealForestPlainsMaxHeight},
                {"occurrence_probability", borealForestPlainsOccurrenceProbability},
                {"evenness", borealForestPlainsEvenness},
                {"tree_density", borealForestPlainsTreeDensity}
            }},
            {"hills", {
                {"max_height", borealForestHillsMaxHeight},
                {"occurrence_probability", borealForestHillsOccurrenceProbability},
                {"bumpiness", borealForestHillsBumpiness},
                {"tree_density", borealForestHillsTreeDensity}
            }},
            {"mountains", {
                {"max_height", borealForestMountainsMaxHeight},
                {"occurrence_probability", borealForestMountainsOccurrenceProbability},
                {"ruggedness", borealForestMountainsRuggedness},
                {"tree_density", borealForestMountainsTreeDensity}
            }}
        }},
    
        {"grassland", {
            {"selected", grasslandSelected},
            {"plains", {
                {"max_height", grasslandPlainsMaxHeight},
                {"occurrence_probability", grasslandPlainsOccurrenceProbability},
                {"evenness", grasslandPlainsEvenness},
                {"tree_density", grasslandPlainsTreeDensity}
            }},
            {"hills", {
                {"max_height", grasslandHillsMaxHeight},
                {"occurrence_probability", grasslandHillsOccurrenceProbability},
                {"bumpiness", grasslandHillsBumpiness},
                {"tree_density", grasslandHillsTreeDensity}
            }},
            {"rocky_fields", {
                {"max_height", grasslandRockyFieldsMaxHeight},
                {"occurrence_probability", grasslandRockyFieldsOccurrenceProbability},
                {"rockiness", grasslandRockyFieldsRockiness},
                {"tree_density", grasslandRockyFieldsTreeDensity}
            }},
            {"terraced_fields", {
                {"max_height", grasslandTerracedFieldsMaxHeight},
                {"occurrence_probability", grasslandTerracedFieldsOccurrenceProbability},
                {"size", grasslandTerracedFieldsSize},
                {"tree_density", grasslandTerracedFieldsTreeDensity},
                {"smoothness", grasslandTerracedFieldsSmoothness},
                {"number_of_terraces", grasslandTerracedFieldsNumberOfTerraces}
            }}
        }},
    
        {"tundra", {
            {"selected", tundraSelected},
            {"plains", {
                {"max_height", tundraPlainsMaxHeight},
                {"occurrence_probability", tundraPlainsOccurrenceProbability},
                {"evenness", tundraPlainsEvenness},
                {"tree_density", tundraPlainsTreeDensity}
            }},
            {"blunt_mountains", {
                {"max_height", tundraBluntMountainsMaxHeight},
                {"occurrence_probability", tundraBluntMountainsOccurrenceProbability},
                {"ruggedness", tundraBluntMountainsRuggedness},
                {"tree_density", tundraBluntMountainsTreeDensity}
            }},
            {"pointy_mountains", {
                {"max_height", tundraPointyMountainsMaxHeight},
                {"occurrence_probability", tundraPointyMountainsOccurrenceProbability},
                {"steepness", tundraPointyMountainsSteepness},
                {"frequency", tundraPointyMountainsFrequency},
                {"tree_density", tundraPointyMountainsTreeDensity}
            }}
        }},
    
        {"savanna", {
            {"selected", savannaSelected},
            {"plains", {
                {"max_height", savannaPlainsMaxHeight},
                {"occurrence_probability", savannaPlainsOccurrenceProbability},
                {"evenness", savannaPlainsEvenness},
                {"tree_density", savannaPlainsTreeDensity}
            }},
            {"mountains", {
                {"max_height", savannaMountainsMaxHeight},
                {"occurrence_probability", savannaMountainsOccurrenceProbability},
                {"ruggedness", savannaMountainsRuggedness},
                {"tree_density", savannaMountainsTreeDensity}
            }}
        }},
    
        {"woodland", {
            {"selected", woodlandSelected},
            {"hills", {
                {"max_height", woodlandHillsMaxHeight},
                {"occurrence_probability", woodlandHillsOccurrenceProbability},
                {"bumpiness", woodlandHillsBumpiness},
                {"tree_density", woodlandHillsTreeDensity}
            }}
        }},
    
        {"tropical_rainforest", {
            {"selected", tropicalRainforestSelected},
            {"plains", {
                {"max_height", tropicalRainforestPlainsMaxHeight},
                {"occurrence_probability", tropicalRainforestPlainsOccurrenceProbability},
                {"evenness", tropicalRainforestPlainsEvenness},
                {"tree_density", tropicalRainforestPlainsTreeDensity}
            }},
            {"mountains", {
                {"max_height", tropicalRainforestMountainsMaxHeight},
                {"occurrence_probability", tropicalRainforestMountainsOccurrenceProbability},
                {"ruggedness", tropicalRainforestMountainsRuggedness},
                {"tree_density", tropicalRainforestMountainsTreeDensity}
            }},
            {"hills", {
                {"max_height", tropicalRainforestHillsMaxHeight},
                {"occurrence_probability", tropicalRainforestHillsOccurrenceProbability},
                {"bumpiness", tropicalRainforestHillsBumpiness},
                {"tree_density", tropicalRainforestHillsTreeDensity}
            }},
            {"volcanoes", {
                {"max_height", tropicalRainforestVolcanoesMaxHeight},
                {"occurrence_probability", tropicalRainforestVolcanoesOccurrenceProbability},
                {"size", tropicalRainforestVolcanoesSize},
                {"tree_density", tropicalRainforestVolcanoesTreeDensity},
                {"thickness", tropicalRainforestVolcanoesThickness},
                {"density", tropicalRainforestVolcanoesDensity}
            }}
        }},
    
        {"temperate_rainforest", {
            {"selected", temperateRainforestSelected},
            {"hills", {
                {"max_height", temperateRainforestHillsMaxHeight},
                {"occurrence_probability", temperateRainforestHillsOccurrenceProbability},
                {"bumpiness", temperateRainforestHillsBumpiness},
                {"tree_density", temperateRainforestHillsTreeDensity}
            }},
            {"mountains", {
                {"max_height", temperateRainforestMountainsMaxHeight},
                {"occurrence_probability", temperateRainforestMountainsOccurrenceProbability},
                {"ruggedness", temperateRainforestMountainsRuggedness},
                {"tree_density", temperateRainforestMountainsTreeDensity}
            }},
            {"swamp", {
                {"max_height", temperateRainforestSwampMaxHeight},
                {"occurrence_probability", temperateRainforestSwampOccurrenceProbability},
                {"wetness", temperateRainforestSwampWetness},
                {"tree_density", temperateRainforestSwampTreeDensity}
            }}
        }},
    
        {"temperate_seasonal_forest", {
            {"selected", temperateSeasonalForestSelected},
            {"hills", {
                {"max_height", temperateSeasonalForestHillsMaxHeight},
                {"occurrence_probability", temperateSeasonalForestHillsOccurrenceProbability},
                {"bumpiness", temperateSeasonalForestHillsBumpiness},
                {"tree_density", temperateSeasonalForestHillsTreeDensity},
                {"autumnal_occurrence", temperateSeasonalForestHillsAutumnalOccurrence}
            }},
            {"mountains", {
                {"max_height", temperateSeasonalForestMountainsMaxHeight},
                {"occurrence_probability", temperateSeasonalForestMountainsOccurrenceProbability},
                {"ruggedness", temperateSeasonalForestMountainsRuggedness},
                {"tree_density", temperateSeasonalForestMountainsTreeDensity},
                {"autumnal_occurrence", temperateSeasonalForestMountainsAutumnalOccurrence}
            }}
        }},
    
        {"subtropical_desert", {
            {"selected", subtropicalDesertSelected},
            {"dunes", {
                {"max_height", subtropicalDesertDunesMaxHeight},
                {"occurrence_probability", subtropicalDesertDunesOccurrenceProbability},
                {"size", subtropicalDesertDunesSize},
                {"tree_density", subtropicalDesertDunesTreeDensity},
                {"dune_frequency", subtropicalDesertDunesDuneFrequency},
                {"dune_waviness", subtropicalDesertDunesDuneWaviness},
                {"bumpiness", subtropicalDesertDunesBumpiness}
            }},
            {"mesas", {
                {"max_height", subtropicalDesertMesasMaxHeight},
                {"occurrence_probability", subtropicalDesertMesasOccurrenceProbability},
                {"size", subtropicalDesertMesasSize},
                {"tree_density", subtropicalDesertMesasTreeDensity},
                {"number_of_terraces", subtropicalDesertMesasNumberOfTerraces},
                {"steepness", subtropicalDesertMesasSteepness}
            }},
            {"ravines", {
                {"max_height", subtropicalDesertRavinesMaxHeight},
                {"occurrence_probability", subtropicalDesertRavinesOccurrenceProbability},
                {"density", subtropicalDesertRavinesDensity},
                {"tree_density", subtropicalDesertRavinesTreeDensity},
                {"ravine_width", subtropicalDesertRavinesRavineWidth},
                {"smoothness", subtropicalDesertRavinesSmoothness},
                {"steepness", subtropicalDesertRavinesSteepness}
            }},
            {"oasis", {
                {"max_height", subtropicalDesertOasisMaxHeight},
                {"occurrence_probability", subtropicalDesertOasisOccurrenceProbability},
                {"size", subtropicalDesertOasisSize},
                {"flatness", subtropicalDesertOasisFlatness},
                {"tree_density", subtropicalDesertOasisTreeDensity},
                {"dune_frequency", subtropicalDesertOasisDuneFrequency}
            }},
            {"cracked", {
                {"max_height", subtropicalDesertCrackedMaxHeight},
                {"occurrence_probability", subtropicalDesertCrackedOccurrenceProbability},
                {"size", subtropicalDesertCrackedSize},
                {"flatness", subtropicalDesertCrackedFlatness},
                {"tree_density", subtropicalDesertCrackedTreeDensity}
            }}
        }},
    
        {"ocean", {
            {"flat_seabed", {
                {"max_height", oceanFlatSeabedMaxHeight},
                {"evenness", oceanFlatSeabedEvenness},
                {"occurrence_probability", oceanFlatSeabedOccurrenceProbability}
            }},
            {"volcanic_islands", {
                {"max_height", oceanVolcanicIslandsMaxHeight},
                {"occurrence_probability", oceanVolcanicIslandsOccurrenceProbability},
                {"size", oceanVolcanicIslandsSize},
                {"thickness", oceanVolcanicIslandsThickness},
                {"density", oceanVolcanicIslandsDensity}
            }},
            {"water_stacks", {
                {"max_height", oceanWaterStacksMaxHeight},
                {"occurrence_probability", oceanWaterStacksOccurrenceProbability},
                {"size", oceanWaterStacksSize}
            }},
            {"trenches", {
                {"density", oceanTrenchesDensity},
                {"occurrence_probability", oceanTrenchesOccurrenceProbability},
                {"trench_width", oceanTrenchesTrenchWidth},
                {"smoothness", oceanTrenchesSmoothness}
            }}
        }},
        {"textures", {
            {"boreal", {
                {"low", borealTextureLow},
                {"mid_flat", borealTextureMidFlat},
                {"mid_steep", borealTextureMidSteep},
                {"high", borealTextureHigh}
            }},
            {"grassy", {
                {"low", grassyTextureLow},
                {"mid_flat", grassyTextureMidFlat},
                {"mid_steep", grassyTextureMidSteep},
                {"high", grassyTextureHigh}
            }},
            {"grassy_stone", {
                {"low", grassyStoneTextureLow},
                {"mid_flat", grassyStoneTextureMidFlat},
                {"mid_steep", grassyStoneTextureMidSteep},
                {"high", grassyStoneTextureHigh}
            }},
            {"snowy", {
                {"low", snowyTextureLow},
                {"mid_flat", snowyTextureMidFlat},
                {"mid_steep", snowyTextureMidSteep},
                {"high", snowyTextureHigh}
            }},
            {"icy", {
                {"low", icyTextureLow},
                {"mid_flat", icyTextureMidFlat},
                {"mid_steep", icyTextureMidSteep},
                {"high", icyTextureHigh}
            }},
            {"savanna", {
                {"low", savannaTextureLow},
                {"mid_flat", savannaTextureMidFlat},
                {"mid_steep", savannaTextureMidSteep},
                {"high", savannaTextureHigh}
            }},
            {"woodland", {
                {"low", woodlandTextureLow},
                {"mid_flat", woodlandTextureMidFlat},
                {"mid_steep", woodlandTextureMidSteep},
                {"high", woodlandTextureHigh}
            }},
            {"jungle", {
                {"low", jungleTextureLow},
                {"mid_flat", jungleTextureMidFlat},
                {"mid_steep", jungleTextureMidSteep},
                {"high", jungleTextureHigh}
            }},
            {"jungle_mountains", {
                {"low", jungleMountainsTextureLow},
                {"mid_flat", jungleMountainsTextureMidFlat},
                {"mid_steep", jungleMountainsTextureMidSteep},
                {"high", jungleMountainsTextureHigh}
            }},
            {"volcanic", {
                {"low", volcanicTextureLow},
                {"mid_flat", volcanicTextureMidFlat},
                {"mid_steep", volcanicTextureMidSteep},
                {"high", volcanicTextureHigh}
            }},
            {"temperate", {
                {"low", temperateTextureLow},
                {"mid_flat", temperateTextureMidFlat},
                {"mid_steep", temperateTextureMidSteep},
                {"high", temperateTextureHigh}
            }},
            {"swamp", {
                {"low", swampTextureLow},
                {"mid_flat", swampTextureMidFlat},
                {"mid_steep", swampTextureMidSteep},
                {"high", swampTextureHigh}
            }},
            {"seasonal_forest", {
                {"low", seasonalForestTextureLow},
                {"mid_flat", seasonalForestTextureMidFlat},
                {"mid_steep", seasonalForestTextureMidSteep},
                {"high", seasonalForestTextureHigh}
            }},
            {"autumnal_forest", {
                {"low", autumnTextureLow},
                {"mid_flat", autumnTextureMidFlat},
                {"mid_steep", autumnTextureMidSteep},
                {"high", autumnTextureHigh}
            }},
            {"mesa_desert", {
                {"low", mesaTextureLow},
                {"mid_flat", mesaTextureMidFlat},
                {"mid_steep", mesaTextureMidSteep},
                {"high", mesaTextureHigh}
            }},
            {"hot_desert", {
                {"low", hotDesertTextureLow},
                {"mid_flat", hotDesertTextureMidFlat},
                {"mid_steep", hotDesertTextureMidSteep},
                {"high", hotDesertTextureHigh}
            }},
            {"dusty_desert", {
                {"low", dustyTextureLow},
                {"mid_flat", dustyTextureMidFlat},
                {"mid_steep", dustyTextureMidSteep},
                {"high", dustyTextureHigh}
            }},
            {"badlands", {
                {"low", badlandsTextureLow},
                {"mid_flat", badlandsTextureMidFlat},
                {"mid_steep", badlandsTextureMidSteep},
                {"high", badlandsTextureHigh}
            }},
            {"oasis", {
                {"low", oasisTextureLow},
                {"mid_flat", oasisTextureMidFlat},
                {"mid_steep", oasisTextureMidSteep},
                {"high", oasisTextureHigh}
            }},
            {"ocean", {
                {"low", oceanTextureLow},
                {"mid_flat", oceanTextureMidFlat},
                {"mid_steep", oceanTextureMidSteep},
                {"high", oceanTextureHigh}
            }},
            {"cliffs", {
                {"low", cliffsTextureLow},
                {"mid_flat", cliffsTextureMidFlat},
                {"mid_steep", cliffsTextureMidSteep},
                {"high", cliffsTextureHigh}
            }}
        }}
    };
    

    string projectRoot = getenv("PROJECT_ROOT");
    string saveDirectory = projectRoot + filePathDelimitter + "saves" + filePathDelimitter + fileName + filePathDelimitter;
    

    // Check if the directory exists, if not create it
    if (!fs::exists(saveDirectory)) {
        fs::create_directories(saveDirectory);
    }

    //  Open the file for writing
    ofstream file(saveDirectory + fileName + ".json");

     // Check if the file opened successfully
    if (!file) {
        cerr << "Error: Unable to open file for saving: " << fileName << endl;
        return false;
    }
    file << jsonData.dump(4);
    file.close();
    return true;
}


// Function to load parameters from a JSON file
void Parameters::loadFromFile(string fileName, char filePathDelimitter) {
    string projectRoot = getenv("PROJECT_ROOT");
    ifstream file(projectRoot + filePathDelimitter + "saves" + filePathDelimitter + fileName + filePathDelimitter + fileName + ".json");
    if (!file) {
        cerr << "Error: Unable to open file for loading: " << fileName << endl;
        return;
    }
    json jsonData;
    file >> jsonData;
    file.close();

    seed = jsonData["seed"];
    maxHeight = jsonData["global_max_height"];
    oceanCoverage = jsonData["ocean_coverage"];
    continentSize = jsonData["continent_size"];
    roughness = jsonData["roughness"];
    mountainousness = jsonData["mountainousness"];
    coastlineRoughness = jsonData["coastline_roughness"];
    biomeSize = jsonData["biome_size"];
    warmth = jsonData["warmth"];
    wetness = jsonData["wetness"];
    riverFrequency = jsonData["river_frequency"];
    riverWidth = jsonData["river_width"];
    riverDepth = jsonData["river_depth"];
    riverMeandering = jsonData["river_meandering"];
    riverSmoothness = jsonData["river_smoothness"];
    treesDensity = jsonData["trees_density"];
    borealForestSelected = jsonData["boreal_forest"]["selected"];
    borealForestPlainsMaxHeight = jsonData["boreal_forest"]["plains"]["max_height"];
    borealForestPlainsOccurrenceProbability = jsonData["boreal_forest"]["plains"]["occurrence_probability"];
    borealForestPlainsEvenness = jsonData["boreal_forest"]["plains"]["evenness"];
    borealForestPlainsTreeDensity = jsonData["boreal_forest"]["plains"]["tree_density"];
    borealForestHillsMaxHeight = jsonData["boreal_forest"]["hills"]["max_height"];
    borealForestHillsOccurrenceProbability = jsonData["boreal_forest"]["hills"]["occurrence_probability"];
    borealForestHillsBumpiness = jsonData["boreal_forest"]["hills"]["bumpiness"];
    borealForestHillsTreeDensity = jsonData["boreal_forest"]["hills"]["tree_density"];
    borealForestMountainsMaxHeight = jsonData["boreal_forest"]["mountains"]["max_height"];
    borealForestMountainsOccurrenceProbability = jsonData["boreal_forest"]["mountains"]["occurrence_probability"];
    borealForestMountainsRuggedness = jsonData["boreal_forest"]["mountains"]["ruggedness"];
    borealForestMountainsTreeDensity = jsonData["boreal_forest"]["mountains"]["tree_density"];
    grasslandSelected = jsonData["grassland"]["selected"];
    grasslandPlainsMaxHeight = jsonData["grassland"]["plains"]["max_height"];
    grasslandPlainsOccurrenceProbability = jsonData["grassland"]["plains"]["occurrence_probability"];
    grasslandPlainsEvenness = jsonData["grassland"]["plains"]["evenness"];
    grasslandPlainsTreeDensity = jsonData["grassland"]["plains"]["tree_density"];
    grasslandHillsMaxHeight = jsonData["grassland"]["hills"]["max_height"];
    grasslandHillsOccurrenceProbability = jsonData["grassland"]["hills"]["occurrence_probability"];
    grasslandHillsBumpiness = jsonData["grassland"]["hills"]["bumpiness"];
    grasslandHillsTreeDensity = jsonData["grassland"]["hills"]["tree_density"];
    grasslandRockyFieldsMaxHeight = jsonData["grassland"]["rocky_fields"]["max_height"];
    grasslandRockyFieldsOccurrenceProbability = jsonData["grassland"]["rocky_fields"]["occurrence_probability"];
    grasslandRockyFieldsRockiness = jsonData["grassland"]["rocky_fields"]["rockiness"];
    grasslandRockyFieldsTreeDensity = jsonData["grassland"]["rocky_fields"]["tree_density"];
    grasslandTerracedFieldsMaxHeight = jsonData["grassland"]["terraced_fields"]["max_height"];
    grasslandTerracedFieldsOccurrenceProbability = jsonData["grassland"]["terraced_fields"]["occurrence_probability"];
    grasslandTerracedFieldsSize = jsonData["grassland"]["terraced_fields"]["size"];
    grasslandTerracedFieldsTreeDensity = jsonData["grassland"]["terraced_fields"]["tree_density"];
    grasslandTerracedFieldsSmoothness = jsonData["grassland"]["terraced_fields"]["smoothness"];
    grasslandTerracedFieldsNumberOfTerraces = jsonData["grassland"]["terraced_fields"]["number_of_terraces"];
    tundraSelected = jsonData["tundra"]["selected"];
    tundraPlainsMaxHeight = jsonData["tundra"]["plains"]["max_height"];
    tundraPlainsOccurrenceProbability = jsonData["tundra"]["plains"]["occurrence_probability"];
    tundraPlainsEvenness = jsonData["tundra"]["plains"]["evenness"];
    tundraPlainsTreeDensity = jsonData["tundra"]["plains"]["tree_density"];
    tundraBluntMountainsMaxHeight = jsonData["tundra"]["blunt_mountains"]["max_height"];
    tundraBluntMountainsOccurrenceProbability = jsonData["tundra"]["blunt_mountains"]["occurrence_probability"];
    tundraBluntMountainsRuggedness = jsonData["tundra"]["blunt_mountains"]["ruggedness"];
    tundraBluntMountainsTreeDensity = jsonData["tundra"]["blunt_mountains"]["tree_density"];
    tundraPointyMountainsMaxHeight = jsonData["tundra"]["pointy_mountains"]["max_height"];
    tundraPointyMountainsOccurrenceProbability = jsonData["tundra"]["pointy_mountains"]["occurrence_probability"];
    tundraPointyMountainsSteepness = jsonData["tundra"]["pointy_mountains"]["steepness"];
    tundraPointyMountainsFrequency = jsonData["tundra"]["pointy_mountains"]["frequency"];
    tundraPointyMountainsTreeDensity = jsonData["tundra"]["pointy_mountains"]["tree_density"];
    savannaSelected = jsonData["savanna"]["selected"];
    savannaPlainsMaxHeight = jsonData["savanna"]["plains"]["max_height"];
    savannaPlainsOccurrenceProbability = jsonData["savanna"]["plains"]["occurrence_probability"];
    savannaPlainsEvenness = jsonData["savanna"]["plains"]["evenness"];
    savannaPlainsTreeDensity = jsonData["savanna"]["plains"]["tree_density"];
    savannaMountainsMaxHeight = jsonData["savanna"]["mountains"]["max_height"];
    savannaMountainsOccurrenceProbability = jsonData["savanna"]["mountains"]["occurrence_probability"];
    savannaMountainsRuggedness = jsonData["savanna"]["mountains"]["ruggedness"];
    savannaMountainsTreeDensity = jsonData["savanna"]["mountains"]["tree_density"];
    woodlandSelected = jsonData["woodland"]["selected"];
    woodlandHillsMaxHeight = jsonData["woodland"]["hills"]["max_height"];
    woodlandHillsOccurrenceProbability = jsonData["woodland"]["hills"]["occurrence_probability"];
    woodlandHillsBumpiness = jsonData["woodland"]["hills"]["bumpiness"];
    woodlandHillsTreeDensity = jsonData["woodland"]["hills"]["tree_density"];
    tropicalRainforestSelected = jsonData["tropical_rainforest"]["selected"];
    tropicalRainforestPlainsMaxHeight = jsonData["tropical_rainforest"]["plains"]["max_height"];
    tropicalRainforestPlainsOccurrenceProbability = jsonData["tropical_rainforest"]["plains"]["occurrence_probability"];
    tropicalRainforestPlainsEvenness = jsonData["tropical_rainforest"]["plains"]["evenness"];
    tropicalRainforestPlainsTreeDensity = jsonData["tropical_rainforest"]["plains"]["tree_density"];
    tropicalRainforestMountainsMaxHeight = jsonData["tropical_rainforest"]["mountains"]["max_height"];
    tropicalRainforestMountainsOccurrenceProbability = jsonData["tropical_rainforest"]["mountains"]["occurrence_probability"];
    tropicalRainforestMountainsRuggedness = jsonData["tropical_rainforest"]["mountains"]["ruggedness"];
    tropicalRainforestMountainsTreeDensity = jsonData["tropical_rainforest"]["mountains"]["tree_density"];
    tropicalRainforestHillsMaxHeight = jsonData["tropical_rainforest"]["hills"]["max_height"];
    tropicalRainforestHillsOccurrenceProbability = jsonData["tropical_rainforest"]["hills"]["occurrence_probability"];
    tropicalRainforestHillsBumpiness = jsonData["tropical_rainforest"]["hills"]["bumpiness"];
    tropicalRainforestHillsTreeDensity = jsonData["tropical_rainforest"]["hills"]["tree_density"];
    tropicalRainforestVolcanoesMaxHeight = jsonData["tropical_rainforest"]["volcanoes"]["max_height"];
    tropicalRainforestVolcanoesOccurrenceProbability = jsonData["tropical_rainforest"]["volcanoes"]["occurrence_probability"];
    tropicalRainforestVolcanoesSize = jsonData["tropical_rainforest"]["volcanoes"]["size"];
    tropicalRainforestVolcanoesTreeDensity = jsonData["tropical_rainforest"]["volcanoes"]["tree_density"];
    tropicalRainforestVolcanoesThickness = jsonData["tropical_rainforest"]["volcanoes"]["thickness"];
    tropicalRainforestVolcanoesDensity = jsonData["tropical_rainforest"]["volcanoes"]["density"];
    temperateRainforestSelected = jsonData["temperate_rainforest"]["selected"];
    temperateRainforestHillsMaxHeight = jsonData["temperate_rainforest"]["hills"]["max_height"];
    temperateRainforestHillsOccurrenceProbability = jsonData["temperate_rainforest"]["hills"]["occurrence_probability"];
    temperateRainforestHillsBumpiness = jsonData["temperate_rainforest"]["hills"]["bumpiness"];
    temperateRainforestHillsTreeDensity = jsonData["temperate_rainforest"]["hills"]["tree_density"];
    temperateRainforestMountainsMaxHeight = jsonData["temperate_rainforest"]["mountains"]["max_height"];
    temperateRainforestMountainsOccurrenceProbability = jsonData["temperate_rainforest"]["mountains"]["occurrence_probability"];
    temperateRainforestMountainsRuggedness = jsonData["temperate_rainforest"]["mountains"]["ruggedness"];
    temperateRainforestMountainsTreeDensity = jsonData["temperate_rainforest"]["mountains"]["tree_density"];
    temperateRainforestSwampMaxHeight = jsonData["temperate_rainforest"]["swamp"]["max_height"];
    temperateRainforestSwampOccurrenceProbability = jsonData["temperate_rainforest"]["swamp"]["occurrence_probability"];
    temperateRainforestSwampWetness = jsonData["temperate_rainforest"]["swamp"]["wetness"];
    temperateRainforestSwampTreeDensity = jsonData["temperate_rainforest"]["swamp"]["tree_density"];
    temperateSeasonalForestSelected = jsonData["temperate_seasonal_forest"]["selected"];
    temperateSeasonalForestHillsMaxHeight = jsonData["temperate_seasonal_forest"]["hills"]["max_height"];
    temperateSeasonalForestHillsOccurrenceProbability = jsonData["temperate_seasonal_forest"]["hills"]["occurrence_probability"];
    temperateSeasonalForestHillsBumpiness = jsonData["temperate_seasonal_forest"]["hills"]["bumpiness"];
    temperateSeasonalForestHillsTreeDensity = jsonData["temperate_seasonal_forest"]["hills"]["tree_density"];
    temperateSeasonalForestHillsAutumnalOccurrence = jsonData["temperate_seasonal_forest"]["hills"]["autumnal_occurrence"];
    temperateSeasonalForestMountainsMaxHeight = jsonData["temperate_seasonal_forest"]["mountains"]["max_height"];
    temperateSeasonalForestMountainsOccurrenceProbability = jsonData["temperate_seasonal_forest"]["mountains"]["occurrence_probability"];
    temperateSeasonalForestMountainsRuggedness = jsonData["temperate_seasonal_forest"]["mountains"]["ruggedness"];
    temperateSeasonalForestMountainsTreeDensity = jsonData["temperate_seasonal_forest"]["mountains"]["tree_density"];
    temperateSeasonalForestMountainsAutumnalOccurrence = jsonData["temperate_seasonal_forest"]["mountains"]["autumnal_occurrence"];
    subtropicalDesertSelected = jsonData["subtropical_desert"]["selected"];
    subtropicalDesertDunesMaxHeight = jsonData["subtropical_desert"]["dunes"]["max_height"];
    subtropicalDesertDunesOccurrenceProbability = jsonData["subtropical_desert"]["dunes"]["occurrence_probability"];
    subtropicalDesertDunesSize = jsonData["subtropical_desert"]["dunes"]["size"];
    subtropicalDesertDunesTreeDensity = jsonData["subtropical_desert"]["dunes"]["tree_density"];
    subtropicalDesertDunesDuneFrequency = jsonData["subtropical_desert"]["dunes"]["dune_frequency"];
    subtropicalDesertDunesDuneWaviness = jsonData["subtropical_desert"]["dunes"]["dune_waviness"];
    subtropicalDesertDunesBumpiness = jsonData["subtropical_desert"]["dunes"]["bumpiness"];
    subtropicalDesertMesasMaxHeight = jsonData["subtropical_desert"]["mesas"]["max_height"];
    subtropicalDesertMesasOccurrenceProbability = jsonData["subtropical_desert"]["mesas"]["occurrence_probability"];
    subtropicalDesertMesasSize = jsonData["subtropical_desert"]["mesas"]["size"];
    subtropicalDesertMesasTreeDensity = jsonData["subtropical_desert"]["mesas"]["tree_density"];
    subtropicalDesertMesasNumberOfTerraces = jsonData["subtropical_desert"]["mesas"]["number_of_terraces"];
    subtropicalDesertMesasSteepness = jsonData["subtropical_desert"]["mesas"]["steepness"];
    subtropicalDesertRavinesMaxHeight = jsonData["subtropical_desert"]["ravines"]["max_height"];
    subtropicalDesertRavinesOccurrenceProbability = jsonData["subtropical_desert"]["ravines"]["occurrence_probability"];
    subtropicalDesertRavinesDensity = jsonData["subtropical_desert"]["ravines"]["density"];
    subtropicalDesertRavinesTreeDensity = jsonData["subtropical_desert"]["ravines"]["tree_density"];
    subtropicalDesertRavinesRavineWidth = jsonData["subtropical_desert"]["ravines"]["ravine_width"];
    subtropicalDesertRavinesSmoothness = jsonData["subtropical_desert"]["ravines"]["smoothness"];
    subtropicalDesertRavinesSteepness = jsonData["subtropical_desert"]["ravines"]["steepness"];
    subtropicalDesertOasisMaxHeight = jsonData["subtropical_desert"]["oasis"]["max_height"];
    subtropicalDesertOasisOccurrenceProbability = jsonData["subtropical_desert"]["oasis"]["occurrence_probability"];
    subtropicalDesertOasisSize = jsonData["subtropical_desert"]["oasis"]["size"];
    subtropicalDesertOasisFlatness = jsonData["subtropical_desert"]["oasis"]["flatness"];
    subtropicalDesertOasisTreeDensity = jsonData["subtropical_desert"]["oasis"]["tree_density"];
    subtropicalDesertOasisDuneFrequency = jsonData["subtropical_desert"]["oasis"]["dune_frequency"];
    subtropicalDesertCrackedMaxHeight = jsonData["subtropical_desert"]["cracked"]["max_height"];
    subtropicalDesertCrackedOccurrenceProbability = jsonData["subtropical_desert"]["cracked"]["occurrence_probability"];
    subtropicalDesertCrackedSize = jsonData["subtropical_desert"]["cracked"]["size"];
    subtropicalDesertCrackedFlatness = jsonData["subtropical_desert"]["cracked"]["flatness"];
    subtropicalDesertCrackedTreeDensity = jsonData["subtropical_desert"]["cracked"]["tree_density"];
    oceanFlatSeabedMaxHeight = jsonData["ocean"]["flat_seabed"]["max_height"];
    oceanFlatSeabedEvenness = jsonData["ocean"]["flat_seabed"]["evenness"];
    oceanFlatSeabedOccurrenceProbability = jsonData["ocean"]["flat_seabed"]["occurrence_probability"];
    oceanVolcanicIslandsMaxHeight = jsonData["ocean"]["volcanic_islands"]["max_height"];
    oceanVolcanicIslandsOccurrenceProbability = jsonData["ocean"]["volcanic_islands"]["occurrence_probability"];
    oceanVolcanicIslandsSize = jsonData["ocean"]["volcanic_islands"]["size"];
    oceanVolcanicIslandsThickness = jsonData["ocean"]["volcanic_islands"]["thickness"];
    oceanVolcanicIslandsDensity = jsonData["ocean"]["volcanic_islands"]["density"];
    oceanWaterStacksMaxHeight = jsonData["ocean"]["water_stacks"]["max_height"];
    oceanWaterStacksOccurrenceProbability = jsonData["ocean"]["water_stacks"]["occurrence_probability"];
    oceanWaterStacksSize = jsonData["ocean"]["water_stacks"]["size"];
    oceanTrenchesDensity = jsonData["ocean"]["trenches"]["density"];
    oceanTrenchesOccurrenceProbability = jsonData["ocean"]["trenches"]["occurrence_probability"];
    oceanTrenchesTrenchWidth = jsonData["ocean"]["trenches"]["trench_width"];
    oceanTrenchesSmoothness = jsonData["ocean"]["trenches"]["smoothness"];
    borealTextureLow = jsonData["textures"]["boreal"]["low"];
    borealTextureMidFlat = jsonData["textures"]["boreal"]["mid_flat"];
    borealTextureMidSteep = jsonData["textures"]["boreal"]["mid_steep"];
    borealTextureHigh = jsonData["textures"]["boreal"]["high"];
    grassyTextureLow = jsonData["textures"]["grassy"]["low"];
    grassyTextureMidFlat = jsonData["textures"]["grassy"]["mid_flat"];
    grassyTextureMidSteep = jsonData["textures"]["grassy"]["mid_steep"];
    grassyTextureHigh = jsonData["textures"]["grassy"]["high"];
    grassyStoneTextureLow = jsonData["textures"]["grassy_stone"]["low"];
    grassyStoneTextureMidFlat = jsonData["textures"]["grassy_stone"]["mid_flat"];
    grassyStoneTextureMidSteep = jsonData["textures"]["grassy_stone"]["mid_steep"];
    grassyStoneTextureHigh = jsonData["textures"]["grassy_stone"]["high"];
    snowyTextureLow = jsonData["textures"]["snowy"]["low"];
    snowyTextureMidFlat = jsonData["textures"]["snowy"]["mid_flat"];
    snowyTextureMidSteep = jsonData["textures"]["snowy"]["mid_steep"];
    snowyTextureHigh = jsonData["textures"]["snowy"]["high"];
    icyTextureLow = jsonData["textures"]["icy"]["low"];
    icyTextureMidFlat = jsonData["textures"]["icy"]["mid_flat"];
    icyTextureMidSteep = jsonData["textures"]["icy"]["mid_steep"];
    icyTextureHigh = jsonData["textures"]["icy"]["high"]; 
    savannaTextureLow = jsonData["textures"]["savanna"]["low"];
    savannaTextureMidFlat = jsonData["textures"]["savanna"]["mid_flat"];
    savannaTextureMidSteep = jsonData["textures"]["savanna"]["mid_steep"];
    savannaTextureHigh = jsonData["textures"]["savanna"]["high"];
    woodlandTextureLow = jsonData["textures"]["woodland"]["low"];
    woodlandTextureMidFlat = jsonData["textures"]["woodland"]["mid_flat"];
    woodlandTextureMidSteep = jsonData["textures"]["woodland"]["mid_steep"];
    woodlandTextureHigh = jsonData["textures"]["woodland"]["high"];
    jungleTextureLow = jsonData["textures"]["jungle"]["low"];
    jungleTextureMidFlat = jsonData["textures"]["jungle"]["mid_flat"];
    jungleTextureMidSteep = jsonData["textures"]["jungle"]["mid_steep"];
    jungleTextureHigh = jsonData["textures"]["jungle"]["high"];
    jungleMountainsTextureLow = jsonData["textures"]["jungle_mountains"]["low"];
    jungleMountainsTextureMidFlat = jsonData["textures"]["jungle_mountains"]["mid_flat"];
    jungleMountainsTextureMidSteep = jsonData["textures"]["jungle_mountains"]["mid_steep"];
    jungleMountainsTextureHigh = jsonData["textures"]["jungle_mountains"]["high"];
    volcanicTextureLow = jsonData["textures"]["volcanic"]["low"];
    volcanicTextureMidFlat = jsonData["textures"]["volcanic"]["mid_flat"];
    volcanicTextureMidSteep = jsonData["textures"]["volcanic"]["mid_steep"];
    volcanicTextureHigh = jsonData["textures"]["volcanic"]["high"];
    temperateTextureLow = jsonData["textures"]["temperate"]["low"];
    temperateTextureMidFlat = jsonData["textures"]["temperate"]["mid_flat"];
    temperateTextureMidSteep = jsonData["textures"]["temperate"]["mid_steep"];
    temperateTextureHigh = jsonData["textures"]["temperate"]["high"];
    swampTextureLow = jsonData["textures"]["swamp"]["low"];
    swampTextureMidFlat = jsonData["textures"]["swamp"]["mid_flat"];
    swampTextureMidSteep = jsonData["textures"]["swamp"]["mid_steep"];
    swampTextureHigh = jsonData["textures"]["swamp"]["high"];
    seasonalForestTextureLow = jsonData["textures"]["seasonal_forest"]["low"];
    seasonalForestTextureMidFlat = jsonData["textures"]["seasonal_forest"]["mid_flat"];
    seasonalForestTextureMidSteep = jsonData["textures"]["seasonal_forest"]["mid_steep"];
    seasonalForestTextureHigh = jsonData["textures"]["seasonal_forest"]["high"];
    autumnTextureLow = jsonData["textures"]["autumnal_forest"]["low"];
    autumnTextureMidFlat = jsonData["textures"]["autumnal_forest"]["mid_flat"];
    autumnTextureMidSteep = jsonData["textures"]["autumnal_forest"]["mid_steep"];
    autumnTextureHigh = jsonData["textures"]["autumnal_forest"]["high"];
    mesaTextureLow = jsonData["textures"]["mesa_desert"]["low"];
    mesaTextureMidFlat = jsonData["textures"]["mesa_desert"]["mid_flat"];
    mesaTextureMidSteep = jsonData["textures"]["mesa_desert"]["mid_steep"];
    mesaTextureHigh = jsonData["textures"]["mesa_desert"]["high"];
    hotDesertTextureLow = jsonData["textures"]["hot_desert"]["low"];
    hotDesertTextureMidFlat = jsonData["textures"]["hot_desert"]["mid_flat"];
    hotDesertTextureMidSteep = jsonData["textures"]["hot_desert"]["mid_steep"];
    hotDesertTextureHigh = jsonData["textures"]["hot_desert"]["high"];  
    dustyTextureLow = jsonData["textures"]["dusty_desert"]["low"];
    dustyTextureMidFlat = jsonData["textures"]["dusty_desert"]["mid_flat"];
    dustyTextureMidSteep = jsonData["textures"]["dusty_desert"]["mid_steep"];
    dustyTextureHigh = jsonData["textures"]["dusty_desert"]["high"];
    badlandsTextureLow = jsonData["textures"]["badlands"]["low"];
    badlandsTextureMidFlat = jsonData["textures"]["badlands"]["mid_flat"];
    badlandsTextureMidSteep = jsonData["textures"]["badlands"]["mid_steep"];
    badlandsTextureHigh = jsonData["textures"]["badlands"]["high"];
    oasisTextureLow = jsonData["textures"]["oasis"]["low"];
    oasisTextureMidFlat = jsonData["textures"]["oasis"]["mid_flat"];
    oasisTextureMidSteep = jsonData["textures"]["oasis"]["mid_steep"];
    oasisTextureHigh = jsonData["textures"]["oasis"]["high"];
    oceanTextureLow = jsonData["textures"]["ocean"]["low"];
    oceanTextureMidFlat = jsonData["textures"]["ocean"]["mid_flat"];
    oceanTextureMidSteep = jsonData["textures"]["ocean"]["mid_steep"];
    oceanTextureHigh = jsonData["textures"]["ocean"]["high"];
    cliffsTextureLow = jsonData["textures"]["cliffs"]["low"];
    cliffsTextureMidFlat = jsonData["textures"]["cliffs"]["mid_flat"];
    cliffsTextureMidSteep = jsonData["textures"]["cliffs"]["mid_steep"];
    cliffsTextureHigh = jsonData["textures"]["cliffs"]["high"];
}


// Function to find the exact texture file path based on the folder name and texture type
string Parameters::findTextureFilePath(string folderName, char filePathDelimitter, string type) {
    string mainTextureRoot = getenv("MAIN_TEXTURE_ROOT");
    for (const auto& entry : fs::directory_iterator(mainTextureRoot + filePathDelimitter + folderName)) {
        std::string filename = entry.path().filename().string();
        if (filename.find(type) != std::string::npos) {
            return entry.path().string(); 
        }
    }
    return "";
}