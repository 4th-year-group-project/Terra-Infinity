#include "Parameters.hpp"
#include <cstdlib>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;
namespace fs = std::filesystem;

// Constructor with default values
Parameters::Parameters()
    : Parameters(
        50, 80, 70, 60, 48, 31, 30, 40, 20, 10, 70, 48, 
        20, 65, 43, 12, 69, 53, 34, 29, 13, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        "coast_sand_01_1k", "rocky_terrain_02_1k", "rock_05_1k", "snow_02_1k", // Temperate rainforest
        "coast_sand_01_1k", "rocky_terrain_02_1k", "rock_05_1k", "snow_02_1k", // Boreal forest
        "coast_sand_01_1k", "rocky_terrain_02_1k", "rock_05_1k", "snow_02_1k", // Grassland
        "coast_sand_01_1k", "rocky_terrain_02_1k", "rock_05_1k", "snow_02_1k", // Tundra
        "coast_sand_01_1k", "rocky_terrain_02_1k", "rock_05_1k", "snow_02_1k", // Savanna
        "coast_sand_01_1k", "rocky_terrain_02_1k", "rock_05_1k", "snow_02_1k", // Woodland
        "coast_sand_01_1k", "rocky_terrain_02_1k", "rock_05_1k", "snow_02_1k", // Tropical rainforest
        "coast_sand_01_1k", "rocky_terrain_02_1k", "rock_05_1k", "snow_02_1k", // Temperate forest
        "coast_sand_01_1k", "rocky_terrain_02_1k", "rock_05_1k", "snow_02_1k" // Subtropical Desert
    ) {}

// Function to set default values
void Parameters::setDefaultValues() {
    *this = Parameters();
}


bool Parameters::saveToFile(string fileName, char filePathDelimitter) {
    json jsonData = {
        {"maximumHeight", maximumHeight},
        {"seaLevel", seaLevel},
        {"oceanCoverage", oceanCoverage},
        {"continentSize", continentSize},
        {"riverWidth", riverWidth},
        {"riverDepth", riverDepth},
        {"riverFrequency", riverFrequency},
        {"riverLength", riverLength},
        {"riverVariety", riverVariety},
        {"lakeFrequency", lakeFrequency},
        {"treesDensity", treesDensity},
        {"treeVariety", treeVariety},
        {"treeHeight", treeHeight},
        {"treeWidth", treeWidth},
        {"treeBiomeExclusivity", treeBiomeExclusivity},
        {"grassDensity", grassDensity},
        {"grassVariety", grassVariety},
        {"bushDensity", bushDensity},
        {"bushVariety", bushVariety},
        {"bushFrequency", bushFrequency},
        {"heightVariance", heightVariance},
        {"warmCold", warmCold},
        {"dryWet", dryWet},
        {"biomeSize", biomeSize},
        {"biomeTerrainVariety", biomeTerrainVariety},
        {"mountainDensity", mountainDensity},
        {"roughness", roughness},
        {"biomeBorderRoughness", biomeBorderRoughness},
        {"coastlineRoughness", coastlineRoughness},
        {"desertProbability", desertProbability},
        {"temperateForestProbability", temperateForestProbability},
        {"tropicalRainforestProbability", tropicalRainforestProbability},
        {"savannaProbability", savannaProbability},
        {"temperateRainforestProbability", temperateRainforestProbability},
        {"borealForestProbability", borealForestProbability},
        {"grasslandProbability", grasslandProbability},
        {"woodlandProbability", woodlandProbability},
        {"tundraProbability", tundraProbability},
        {"desertTexture1", desertTexture1},
        {"desertTexture2", desertTexture2},
        {"desertTexture3", desertTexture3},
        {"desertTexture4", desertTexture4},
        {"temperateForestTexture1", temperateForestTexture1},
        {"temperateForestTexture2", temperateForestTexture2},
        {"temperateForestTexture3", temperateForestTexture3},
        {"temperateForestTexture4", temperateForestTexture4},
        {"tropicalRainforestTexture1", tropicalRainforestTexture1},
        {"tropicalRainforestTexture2", tropicalRainforestTexture2},
        {"tropicalRainforestTexture3", tropicalRainforestTexture3},
        {"tropicalRainforestTexture4", tropicalRainforestTexture4},
        {"borealForestTexture1", borealForestTexture1},
        {"borealForestTexture2", borealForestTexture2},
        {"borealForestTexture3", borealForestTexture3},
        {"borealForestTexture4", borealForestTexture4},
        {"grasslandTexture1", grasslandTexture1},
        {"grasslandTexture2", grasslandTexture2},
        {"grasslandTexture3", grasslandTexture3},
        {"grasslandTexture4", grasslandTexture4},
        {"woodlandTexture1", woodlandTexture1},
        {"woodlandTexture2", woodlandTexture2},
        {"woodlandTexture3", woodlandTexture3},
        {"woodlandTexture4", woodlandTexture4},
        {"tundraTexture1", tundraTexture1},
        {"tundraTexture2", tundraTexture2},
        {"tundraTexture3", tundraTexture3},
        {"tundraTexture4", tundraTexture4},
        {"temperateRainforestTexture1", temperateRainforestTexture1},
        {"temperateRainforestTexture2", temperateRainforestTexture2},
        {"temperateRainforestTexture3", temperateRainforestTexture3},
        {"temperateRainforestTexture4", temperateRainforestTexture4},
        {"savannaTexture1", savannaTexture1},
        {"savannaTexture2", savannaTexture2},
        {"savannaTexture3", savannaTexture3},
        {"savannaTexture4", savannaTexture4}
    };

    string dataRoot = getenv("DATA_ROOT");
    ofstream file(dataRoot + filePathDelimitter + "saved" + filePathDelimitter + fileName);
    if (!file) {
        cerr << "Error: Unable to open file for saving: " << fileName << endl;
        return false;
    }
    file << jsonData.dump(4);
    file.close();
    return true;
}

void Parameters::loadFromFile(string fileName, char filePathDelimitter) {
    string dataRoot = getenv("DATA_ROOT");
    ifstream file(dataRoot + filePathDelimitter + "saved" + filePathDelimitter + fileName);
    if (!file) {
        cerr << "Error: Unable to open file for loading: " << fileName << endl;
        return;
    }
    json jsonData;
    file >> jsonData;
    file.close();

    maximumHeight = jsonData["maximumHeight"];
    seaLevel = jsonData["seaLevel"];
    oceanCoverage = jsonData["oceanCoverage"];
    continentSize = jsonData["continentSize"];
    riverWidth = jsonData["riverWidth"];
    riverDepth = jsonData["riverDepth"];
    riverFrequency = jsonData["riverFrequency"];
    riverLength = jsonData["riverLength"];
    riverVariety = jsonData["riverVariety"];
    lakeFrequency = jsonData["lakeFrequency"];
    treesDensity = jsonData["treesDensity"];
    treeVariety = jsonData["treeVariety"];
    treeHeight = jsonData["treeHeight"];
    treeWidth = jsonData["treeWidth"];
    treeBiomeExclusivity = jsonData["treeBiomeExclusivity"];
    grassDensity = jsonData["grassDensity"];
    grassVariety = jsonData["grassVariety"];
    bushDensity = jsonData["bushDensity"];
    bushVariety = jsonData["bushVariety"];
    bushFrequency = jsonData["bushFrequency"];
    heightVariance = jsonData["heightVariance"];
    warmCold = jsonData["warmCold"];
    dryWet = jsonData["dryWet"];
    biomeSize = jsonData["biomeSize"];
    biomeTerrainVariety = jsonData["biomeTerrainVariety"];
    mountainDensity = jsonData["mountainDensity"];
    roughness = jsonData["roughness"];
    biomeBorderRoughness = jsonData["biomeBorderRoughness"];
    coastlineRoughness = jsonData["coastlineRoughness"];
    desertProbability = jsonData["desertProbability"];
    temperateForestProbability = jsonData["temperateForestProbability"];
    tropicalRainforestProbability = jsonData["tropicalRainforestProbability"];
    savannaProbability = jsonData["savannaProbability"];
    temperateRainforestProbability = jsonData["temperateRainforestProbability"];
    borealForestProbability = jsonData["borealForestProbability"];
    grasslandProbability = jsonData["grasslandProbability"];
    woodlandProbability = jsonData["woodlandProbability"];
    tundraProbability = jsonData["tundraProbability"];
    desertTexture1 = jsonData["desertTexture1"];
    desertTexture2 = jsonData["desertTexture2"];
    desertTexture3 = jsonData["desertTexture3"];
    desertTexture4 = jsonData["desertTexture4"];
    temperateForestTexture1 = jsonData["temperateForestTexture1"];
    temperateForestTexture2 = jsonData["temperateForestTexture2"];
    temperateForestTexture3 = jsonData["temperateForestTexture3"];
    temperateForestTexture4 = jsonData["temperateForestTexture4"];
    tropicalRainforestTexture1 = jsonData["tropicalRainforestTexture1"];
    tropicalRainforestTexture2 = jsonData["tropicalRainforestTexture2"];
    tropicalRainforestTexture3 = jsonData["tropicalRainforestTexture3"];
    tropicalRainforestTexture4 = jsonData["tropicalRainforestTexture4"];
    savannaTexture1 = jsonData["savannaTexture1"];
    savannaTexture2 = jsonData["savannaTexture2"];
    savannaTexture3 = jsonData["savannaTexture3"];  
    savannaTexture4 = jsonData["savannaTexture4"];
    temperateRainforestTexture1 = jsonData["temperateRainforestTexture1"];
    temperateRainforestTexture2 = jsonData["temperateRainforestTexture2"];
    temperateRainforestTexture3 = jsonData["temperateRainforestTexture3"];
    temperateRainforestTexture4 = jsonData["temperateRainforestTexture4"];
    borealForestTexture1 = jsonData["borealForestTexture1"];
    borealForestTexture2 = jsonData["borealForestTexture2"];
    borealForestTexture3 = jsonData["borealForestTexture3"];
    borealForestTexture4 = jsonData["borealForestTexture4"];
    grasslandTexture1 = jsonData["grasslandTexture1"];
    grasslandTexture2 = jsonData["grasslandTexture2"];
    grasslandTexture3 = jsonData["grasslandTexture3"];
    grasslandTexture4 = jsonData["grasslandTexture4"];
    woodlandTexture1 = jsonData["woodlandTexture1"];
    woodlandTexture2 = jsonData["woodlandTexture2"];
    woodlandTexture3 = jsonData["woodlandTexture3"];
    woodlandTexture4 = jsonData["woodlandTexture4"];
    tundraTexture1 = jsonData["tundraTexture1"];
    tundraTexture2 = jsonData["tundraTexture2"];
    tundraTexture3 = jsonData["tundraTexture3"];
    tundraTexture4 = jsonData["tundraTexture4"];
}


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