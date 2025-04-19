#include "Parameters.hpp"
#include <cstdlib>
#include <filesystem>
#include <nlohmann/json.hpp>

#include <chrono>

using json = nlohmann::json;
using namespace std;
namespace fs = std::filesystem;


/**
 * This function will set the default values for all of the parameters that will be used in generating terrain and texturing, including the random seed.
 * @param worldName The name of the world that will be generated. This will be used to set the random seed.
 */
void Parameters::setDefaultValues(string worldName) {
    *this = Parameters(); // Reset to default values
    setRandomSeed(worldName); // Set the random seed based on the world name
}


/**
 * This is the default constructor for the Parameters class. It will set the default values for all of the parameters that will be used in generating terrain and texturing. 
 * The random seed will be set when the world is generated.
 */
Parameters::Parameters() {
    // Global parameters
    seed = 23; // This will be overridden by the setRandomSeed function when generating a new world
    globalMaxHeight = 100;
    oceanCoverage = 50;
    continentSize = 50;
    globalRuggedness = 50;
    biomeSize = 50;
    warmth = 50;
    wetness = 50;
    riverFrequency = 50;
    riverWidth = 50;
    riverDepth = 50;
    riverMeandering = 50;
    globalTreeDensity = 50;

    // Boreal forest
    borealForestSelected = true;
    borealForestPlainsMaxHeight = 30;
    borealForestPlainsOccurrenceProbability = 50;
    borealForestPlainsEvenness = 50;
    borealForestPlainsTreeDensity = 50;
    borealForestHillsMaxHeight = 40;
    borealForestHillsOccurrenceProbability = 50;
    borealForestHillsBumpiness = 50;
    borealForestHillsTreeDensity = 50;
    borealForestMountainsMaxHeight = 70;
    borealForestMountainsOccurrenceProbability = 50;
    borealForestMountainsRuggedness = 100;
    borealForestMountainsTreeDensity = 50;

    // Grassland
    grasslandSelected = true;
    grasslandPlainsMaxHeight = 30;
    grasslandPlainsOccurrenceProbability = 50;
    grasslandPlainsEvenness = 50;
    grasslandPlainsTreeDensity = 50;
    grasslandHillsMaxHeight = 40;
    grasslandHillsOccurrenceProbability = 50;
    grasslandHillsBumpiness = 50;
    grasslandHillsTreeDensity = 50;
    grasslandRockyFieldsMaxHeight = 40;
    grasslandRockyFieldsOccurrenceProbability = 50;
    grasslandRockyFieldsRockiness = 50;
    grasslandRockyFieldsTreeDensity = 50;
    grasslandTerracedFieldsMaxHeight = 40;
    grasslandTerracedFieldsOccurrenceProbability = 30;
    grasslandTerracedFieldsSize = 40;
    grasslandTerracedFieldsTreeDensity = 50;
    grasslandTerracedFieldsSmoothness = 50;
    grasslandTerracedFieldsNumberOfTerraces = 50;

    // Tundra
    tundraSelected = true;
    tundraPlainsMaxHeight = 40;
    tundraPlainsOccurrenceProbability = 50;
    tundraPlainsEvenness = 50;
    tundraPlainsTreeDensity = 50;
    tundraBluntMountainsMaxHeight = 100;
    tundraBluntMountainsOccurrenceProbability = 50;
    tundraBluntMountainsRuggedness = 100;
    tundraBluntMountainsTreeDensity = 50;
    tundraPointyMountainsMaxHeight = 100;
    tundraPointyMountainsOccurrenceProbability = 50;
    tundraPointyMountainsSteepness = 80;
    tundraPointyMountainsFrequency = 80;
    tundraPointyMountainsTreeDensity = 20;

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
    tropicalRainforestPlainsMaxHeight = 40;
    tropicalRainforestPlainsOccurrenceProbability = 50;
    tropicalRainforestPlainsEvenness = 50;
    tropicalRainforestPlainsTreeDensity = 50;
    tropicalRainforestMountainsMaxHeight = 80;
    tropicalRainforestMountainsOccurrenceProbability = 50;
    tropicalRainforestMountainsRuggedness = 100;
    tropicalRainforestMountainsTreeDensity = 50;
    tropicalRainforestHillsMaxHeight = 50;
    tropicalRainforestHillsOccurrenceProbability = 50;
    tropicalRainforestHillsBumpiness = 50;
    tropicalRainforestHillsTreeDensity = 50;
    tropicalRainforestVolcanoesMaxHeight = 60;
    tropicalRainforestVolcanoesOccurrenceProbability = 50;
    tropicalRainforestVolcanoesSize = 100;
    tropicalRainforestVolcanoesTreeDensity = 50;
    tropicalRainforestVolcanoesThickness = 60;
    tropicalRainforestVolcanoesDensity = 80;

    // Temperate Rainforest
    temperateRainforestSelected = true;
    temperateRainforestHillsMaxHeight = 40;
    temperateRainforestHillsOccurrenceProbability = 50;
    temperateRainforestHillsBumpiness = 50;
    temperateRainforestHillsTreeDensity = 50;
    temperateRainforestMountainsMaxHeight = 80;
    temperateRainforestMountainsOccurrenceProbability = 50;
    temperateRainforestMountainsRuggedness = 100;
    temperateRainforestMountainsTreeDensity = 50;
    temperateRainforestSwampMaxHeight = 30;
    temperateRainforestSwampOccurrenceProbability = 50;
    temperateRainforestSwampWetness = 80;
    temperateRainforestSwampTreeDensity = 50;

    // Temperate Seasonal Forest
    temperateSeasonalForestSelected = true;
    temperateSeasonalForestHillsMaxHeight = 40;
    temperateSeasonalForestHillsOccurrenceProbability = 50;
    temperateSeasonalForestHillsBumpiness = 50;
    temperateSeasonalForestHillsTreeDensity = 50;
    temperateSeasonalForestHillsAutumnalOccurrence = 50;
    temperateSeasonalForestMountainsMaxHeight = 80;
    temperateSeasonalForestMountainsOccurrenceProbability = 50;
    temperateSeasonalForestMountainsRuggedness = 100;
    temperateSeasonalForestMountainsTreeDensity = 50;
    temperateSeasonalForestMountainsAutumnalOccurrence = 50;

    // Subtropical Desert
    subtropicalDesertSelected = true;
    subtropicalDesertDunesMaxHeight = 50;
    subtropicalDesertDunesOccurrenceProbability = 50;
    subtropicalDesertDunesSize = 50;
    subtropicalDesertDunesTreeDensity = 50;
    subtropicalDesertDunesDuneFrequency = 30;
    subtropicalDesertDunesDuneWaviness = 80;
    subtropicalDesertDunesBumpiness = 50;
    subtropicalDesertMesasMaxHeight = 90;
    subtropicalDesertMesasOccurrenceProbability = 50;
    subtropicalDesertMesasSize = 50;
    subtropicalDesertMesasTreeDensity = 50;
    subtropicalDesertMesasNumberOfTerraces = 50;
    subtropicalDesertMesasSteepness = 50;
    subtropicalDesertRavinesMaxHeight = 90;
    subtropicalDesertRavinesOccurrenceProbability = 50;
    subtropicalDesertRavinesDensity = 50;
    subtropicalDesertRavinesTreeDensity = 50;
    subtropicalDesertRavinesRavineWidth = 40;
    subtropicalDesertRavinesSmoothness = 50;
    subtropicalDesertRavinesSteepness = 30;
    subtropicalDesertOasisMaxHeight = 30;
    subtropicalDesertOasisOccurrenceProbability = 50;
    subtropicalDesertOasisSize = 50;
    subtropicalDesertOasisFlatness = 50;
    subtropicalDesertOasisTreeDensity = 50;
    subtropicalDesertOasisDuneFrequency = 0;
    subtropicalDesertCrackedMaxHeight = 70;
    subtropicalDesertCrackedOccurrenceProbability = 50;
    subtropicalDesertCrackedSize = 10;
    subtropicalDesertCrackedFlatness = 50;
    subtropicalDesertCrackedTreeDensity = 50;

    // Ocean
    oceanSelected = true;
    oceanFlatSeabedMaxHeight = 50;
    oceanFlatSeabedEvenness = 50;
    oceanFlatSeabedOccurrenceProbability = 50;
    oceanVolcanicIslandsMaxHeight = 50;
    oceanVolcanicIslandsOccurrenceProbability = 50;
    oceanVolcanicIslandsSize = 50;
    oceanVolcanicIslandsThickness = 50;
    oceanVolcanicIslandsDensity = 50;
    oceanWaterStacksMaxHeight = 20;
    oceanWaterStacksOccurrenceProbability = 50;
    oceanWaterStacksSize = 50;
    oceanTrenchesMaxHeight = 50;
    oceanTrenchesDensity = 10;
    oceanTrenchesOccurrenceProbability = 50;
    oceanTrenchesTrenchWidth = 20;
    oceanTrenchesSmoothness = 20;

    // Textures
    borealTextureLow = "ground_0014_1k";
    borealTextureMidFlat = "Ground037_1K-JPG";
    borealTextureMidSteep = "Ground024_1K-JPG";
    borealTextureHigh = "Snow012_1K-JPG";

    grassyTextureLow = "Grass006_1K-JPG";
    grassyTextureMidFlat = "Grass005_1K-JPG";
    grassyTextureMidSteep = "Ground003_1K-JPG";
    grassyTextureHigh = "aerial_rocks_02_1k";

    grassyStoneTextureLow = "Grass006_1K-JPG";
    grassyStoneTextureMidFlat = "Grass005_1K-JPG";
    grassyStoneTextureMidSteep = "Ground024_1K-JPG";
    grassyStoneTextureHigh = "Snow012_1K-JPG";

    snowyTextureLow = "Grass006_1K-JPG";
    snowyTextureMidFlat = "Grass005_1K-JPG";
    snowyTextureMidSteep = "Ground024_1K-JPG";
    snowyTextureHigh = "Snow012_1K-JPG";

    icyTextureLow = "Grass006_1K-JPG";
    icyTextureMidFlat = "Grass005_1K-JPG";
    icyTextureMidSteep = "Ground024_1K-JPG";
    icyTextureHigh = "Snow012_1K-JPG";

    savannaTextureLow = "Grass006_1K-JPG";
    savannaTextureMidFlat = "Grass005_1K-JPG";
    savannaTextureMidSteep = "Ground024_1K-JPG";
    savannaTextureHigh = "Snow012_1K-JPG";

    woodlandTextureLow = "Grass006_1K-JPG";
    woodlandTextureMidFlat = "Grass005_1K-JPG";
    woodlandTextureMidSteep = "Ground024_1K-JPG";
    woodlandTextureHigh = "Snow012_1K-JPG";
    
    jungleTextureLow = "Grass006_1K-JPG";
    jungleTextureMidFlat = "Grass005_1K-JPG";
    jungleTextureMidSteep = "Ground024_1K-JPG";
    jungleTextureHigh = "Snow012_1K-JPG";

    jungleMountainsTextureLow = "Grass006_1K-JPG";
    jungleMountainsTextureMidFlat = "Grass005_1K-JPG";
    jungleMountainsTextureMidSteep = "Ground024_1K-JPG";
    jungleMountainsTextureHigh = "Snow012_1K-JPG";

    volcanicTextureLow = "Grass006_1K-JPG";
    volcanicTextureMidFlat = "Grass005_1K-JPG";
    volcanicTextureMidSteep = "Ground024_1K-JPG";
    volcanicTextureHigh = "Snow012_1K-JPG";

    temperateTextureLow = "Grass006_1K-JPG";
    temperateTextureMidFlat = "Grass005_1K-JPG";
    temperateTextureMidSteep = "Ground024_1K-JPG";
    temperateTextureHigh = "Snow012_1K-JPG";

    swampTextureLow = "Grass006_1K-JPG";
    swampTextureMidFlat = "Grass005_1K-JPG";
    swampTextureMidSteep = "Ground024_1K-JPG";
    swampTextureHigh = "Snow012_1K-JPG";

    seasonalForestTextureLow = "Grass006_1K-JPG";
    seasonalForestTextureMidFlat = "Grass005_1K-JPG";
    seasonalForestTextureMidSteep = "Ground024_1K-JPG";
    seasonalForestTextureHigh = "Snow012_1K-JPG";

    autumnTextureLow = "Grass006_1K-JPG";
    autumnTextureMidFlat = "Grass005_1K-JPG";
    autumnTextureMidSteep = "Ground024_1K-JPG";
    autumnTextureHigh = "Snow012_1K-JPG";

    mesaTextureLow = "Grass006_1K-JPG";
    mesaTextureMidFlat = "Grass005_1K-JPG";
    mesaTextureMidSteep = "Ground024_1K-JPG";
    mesaTextureHigh = "Snow012_1K-JPG";

    hotDesertTextureLow = "Grass006_1K-JPG";
    hotDesertTextureMidFlat = "Grass005_1K-JPG";
    hotDesertTextureMidSteep = "Ground024_1K-JPG";
    hotDesertTextureHigh = "Snow012_1K-JPG";

    dustyTextureLow = "Grass006_1K-JPG";
    dustyTextureMidFlat = "Grass005_1K-JPG";
    dustyTextureMidSteep = "Ground024_1K-JPG";
    dustyTextureHigh = "Snow012_1K-JPG";

    badlandsTextureLow = "Grass006_1K-JPG";
    badlandsTextureMidFlat = "Grass005_1K-JPG";
    badlandsTextureMidSteep = "Ground024_1K-JPG";
    badlandsTextureHigh = "Snow012_1K-JPG";

    oasisTextureLow = "Grass006_1K-JPG";
    oasisTextureMidFlat = "Grass005_1K-JPG";
    oasisTextureMidSteep = "Ground024_1K-JPG";
    oasisTextureHigh = "Snow012_1K-JPG";

    oceanTextureLow = "Grass006_1K-JPG";
    oceanTextureMidFlat = "Grass005_1K-JPG";
    oceanTextureMidSteep = "Ground024_1K-JPG";
    oceanTextureHigh = "Snow012_1K-JPG";

    cliffsTextureLow = "Grass006_1K-JPG";
    cliffsTextureMidFlat = "Grass005_1K-JPG";
    cliffsTextureMidSteep = "Ground024_1K-JPG";
    cliffsTextureHigh = "Snow012_1K-JPG";
}


/**
 * This function will set the random seed for the world generation. It will create a hash from the world's name and use it to set the seed.
 * Currently there is a restriction on the world generation that using np.random.seed will not allow a value greater than 2^32 - 1. 
 * This is a limitation of the numpy libraryand for this reason we are type casting all of our long seeds to uint32_t. 
 * If we find a solution to get around it then we can remove the static cast and use the long type.
 * @param worldName The name of the world that will be generated. This will be used to set the random seed.
*/
void Parameters::setRandomSeed(string worldName){
    // // Get the current time without using time function and initialise srand
    // auto now = chrono::system_clock::now();
    // auto duration = now.time_since_epoch();
    // auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
    // srand(millis);
    // int msbRandom = rand();
    // int lsbRandom = rand();
    // uint64_t u_seed = (static_cast<uint64_t>(msbRandom) << 32) | static_cast<uint64_t>(lsbRandom);
    // seed = static_cast<long>(u_seed);

    // Create a hash from the world's name
    std::hash<std::string> hasher;
    seed = static_cast<uint32_t>(hasher(worldName));
}


/**
 * This function will save the parameters to a file in JSON format.
 * @param fileName The name of the file to save the parameters to.
 * @param filePathDelimitter The delimiter to use for the file path. This is usually '/' or '\\' depending on the operating system.
 * @return true if the file was saved successfully, false otherwise.
 */
bool Parameters::saveToFile(string fileName, char filePathDelimitter) {
    nlohmann::json jsonData = {
        {"seed", seed},
        {"global_max_height", globalMaxHeight},
        {"ocean_coverage", oceanCoverage},
        {"continent_size", continentSize},
        {"global_ruggedness", globalRuggedness},
        {"biome_size", biomeSize},
        {"warmth", warmth},
        {"wetness", wetness},
        {"river_frequency", riverFrequency},
        {"river_width", riverWidth},
        {"river_depth", riverDepth},
        {"river_meandering", riverMeandering},
        {"global_tree_density", globalTreeDensity},

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
            {"selected", oceanSelected},
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
                {"max_height", oceanTrenchesMaxHeight},
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
    

    string projectRoot = getenv("PROJECT_ROOT"); // Get the project root directory from the environment variable
    // Create the saves directory path
    string saveDirectory = projectRoot + filePathDelimitter + "saves" + filePathDelimitter + fileName + filePathDelimitter;
    
    // Check if the saves directory exists, if not create it
    if (!fs::exists(saveDirectory)) {
        fs::create_directories(saveDirectory);
    }

    // Open the file for writing
    ofstream file(saveDirectory + fileName + ".json");

     // Check if the file opened successfully
    if (!file) {
        cerr << "Error: Unable to open file for saving: " << fileName << endl;
        return false;
    }

    file << jsonData.dump(4); // Write the JSON data to the file with 4 spaces indentation
    file.close(); // Close the file
    return true;
}


/**
 * This function will load the parameters from a file in JSON format.
 * @param fileName The name of the file to load the parameters from.
 * @param filePathDelimitter The delimiter to use for the file path. This is usually '/' or '\\' depending on the operating system.
 * @return true if the file was loaded successfully, false otherwise.
 */
void Parameters::loadFromFile(string fileName, char filePathDelimitter) {
    string projectRoot = getenv("PROJECT_ROOT"); // Get the project root directory from the environment variable
    // Create the saves directory path
    ifstream file(projectRoot + filePathDelimitter + "saves" + filePathDelimitter + fileName + filePathDelimitter + fileName + ".json");

    // Check if the file opened successfully
    if (!file) {
        cerr << "Error: Unable to open file for loading: " << fileName << endl;
        return;
    }

    json jsonData;
    file >> jsonData; // Read the JSON data from the file
    file.close(); // Close the file
 
    // Set the parameters from the JSON data
    seed = jsonData["seed"];
    globalMaxHeight = jsonData["global_max_height"];
    oceanCoverage = jsonData["ocean_coverage"];
    continentSize = jsonData["continent_size"];
    globalRuggedness = jsonData["global_ruggedness"];
    biomeSize = jsonData["biome_size"];
    warmth = jsonData["warmth"];
    wetness = jsonData["wetness"];
    riverFrequency = jsonData["river_frequency"];
    riverWidth = jsonData["river_width"];
    riverDepth = jsonData["river_depth"];
    riverMeandering = jsonData["river_meandering"];
    globalTreeDensity = jsonData["global_tree_density"];
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
    oceanSelected = jsonData["ocean"]["selected"];
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
    oceanTrenchesMaxHeight = jsonData["ocean"]["trenches"]["max_height"];
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



/**
 * This function will find the exact texture file path based on the folder name and texture type.
 * @param folderName The name of the folder where the texture files are located.
 * @param filePathDelimitter The delimiter to use for the file path. This is usually '/' or '\\' depending on the operating system.
 * @param type The type of texture to find (e.g., "_diff", "_spec", etc.).
 * @return The full path to the texture file if found, otherwise an empty string.
 */
string Parameters::findTextureFilePath(string folderName, char filePathDelimitter, string type) {
    string mainTextureRoot = getenv("MAIN_TEXTURE_ROOT");
    for (const auto& entry : fs::directory_iterator(mainTextureRoot + filePathDelimitter + folderName)) {
        std::string filename = entry.path().filename().string();
        // Check if the filename contains the specified type
        if (filename.find(type) != std::string::npos) {
            return entry.path().string(); 
        }
    }
    return "";
}