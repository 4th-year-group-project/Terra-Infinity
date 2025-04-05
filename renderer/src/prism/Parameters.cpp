#include "Parameters.hpp"
#include <cstdlib>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

// Constructor with default values
Parameters::Parameters()
    : Parameters(
        50, 10, 80, 70, 60, 48, 31, 30, 40, 20, 
        10, 70, 48, 20, 65, 43, 12, 69, 53, 34,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        "grass.jpg", "grass.jpg", "grass.jpg", "grass.jpg", "grass.jpg",
        "grass.jpg", "grass.jpg", "grass.jpg", "grass.jpg"
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
        {"landFragmentation", landFragmentation},
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
        {"desertTexture", desertTexture},
        {"temperateForestTexture", temperateForestTexture},
        {"tropicalRainforestTexture", tropicalRainforestTexture},
        {"savannaTexture", savannaTexture},
        {"temperateRainforestTexture", temperateRainforestTexture},
        {"borealForestTexture", borealForestTexture},
        {"grasslandTexture", grasslandTexture},
        {"woodlandTexture", woodlandTexture},
        {"tundraTexture", tundraTexture}
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
    landFragmentation = jsonData["landFragmentation"];
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
    desertTexture = jsonData["desertTexture"];
    temperateForestTexture = jsonData["temperateForestTexture"];
    tropicalRainforestTexture = jsonData["tropicalRainforestTexture"];
    savannaTexture = jsonData["savannaTexture"];
    temperateRainforestTexture = jsonData["temperateRainforestTexture"];
    borealForestTexture = jsonData["borealForestTexture"];
    grasslandTexture = jsonData["grasslandTexture"];
    woodlandTexture = jsonData["woodlandTexture"];
    tundraTexture = jsonData["tundraTexture"];
}
