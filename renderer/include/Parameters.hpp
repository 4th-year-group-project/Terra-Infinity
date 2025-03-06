#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

/*
    This class will hold all of the values of parameters the user has chosen that will be used in generating terrain.
*/
class Parameters
{
private:
    int maximumHeight;
    int seaLevel;
    int oceanCoverage;
    int landFragmentation;
    int continentSize;
    int riverWidth;
    int riverDepth;
    int riverFrequency;
    int riverLength;
    int riverVariety;
    int lakeFrequency;

    int treesDensity;
    int treeVariety;
    int treeHeight;
    int treeWidth;
    int treeBiomeExclusivity;

    int grassDensity;
    int grassVariety;

    int bushDensity;
    int bushVariety;
    int bushFrequency;

    int heightVariance;

    int warmCold;
    int dryWet;
    int biomeSize;
    int biomeTerrainVariety;

    int mountainDensity;
    int roughness;
    int biomeBorderRoughness;
    int coastlineRoughness;

    int desertProbability;
    int temperateForestProbability;
    int tropicalRainforestProbability;
    int savannaProbability;
    int temperateRainforestProbability;
    int borealForestProbability;
    int grasslandProbability;
    int woodlandProbability;
    int tundraProbability;

    string desertTexture;
    string temperateForestTexture;
    string tropicalRainforestTexture;
    string savannaTexture;
    string temperateRainforestTexture;
    string borealForestTexture;
    string grasslandTexture;
    string woodlandTexture;
    string tundraTexture;

public:
    // Constructor with parameters
    Parameters(
        int inMaximumHeight,
        int inSeaLevel,
        int inOceanCoverage,
        int inLandFragmentation,
        int inContinentSize,
        int inRiverWidth,
        int inRiverDepth,
        int inRiverFrequency,
        int inRiverLength,
        int inRiverVariety,
        int inLakeFrequency,
        int inTreesDensity,
        int inTreeVariety,
        int inTreeHeight,
        int inTreeWidth,
        int inTreeBiomeExclusivity,
        int inGrassDensity,
        int inGrassVariety,
        int inBushDensity,
        int inBushVariety,
        int inBushFrequency,
        int inHeightVariance,
        int inWarmCold,
        int inDryWet,
        int inBiomeSize,
        int inBiomeTerrainVariety,
        int inMountainDensity,
        int inRoughness,
        int inBiomeBorderRoughness,
        int inCoastlineRoughness,
        int inDesertProbability,
        int inTemperateForestProbability,
        int inTropicalRainforestProbability,
        int inSavannaProbability,
        int inTemperateRainforestProbability,
        int inBorealForestProbability,
        int inGrasslandProbability,
        int inWoodlandProbability,
        int inTundraProbability,
        string inDesertTexture,
        string inTemperateForestTexture,
        string inTropicalRainforestTexture,
        string inSavannaTexture,
        string inTemperateRainforestTexture,
        string inBorealForestTexture,
        string inGrasslandTexture,
        string inWoodlandTexture,
        string inTundraTexture
    ) :
        maximumHeight(inMaximumHeight),
        seaLevel(inSeaLevel),
        oceanCoverage(inOceanCoverage),
        landFragmentation(inLandFragmentation),
        continentSize(inContinentSize),
        riverWidth(inRiverWidth),
        riverDepth(inRiverDepth),
        riverFrequency(inRiverFrequency),
        riverLength(inRiverLength),
        riverVariety(inRiverVariety),
        lakeFrequency(inLakeFrequency),
        treesDensity(inTreesDensity),
        treeVariety(inTreeVariety),
        treeHeight(inTreeHeight),
        treeWidth(inTreeWidth),
        treeBiomeExclusivity(inTreeBiomeExclusivity),
        grassDensity(inGrassDensity),
        grassVariety(inGrassVariety),
        bushDensity(inBushDensity),
        bushVariety(inBushVariety),
        bushFrequency(inBushFrequency),
        heightVariance(inHeightVariance),
        warmCold(inWarmCold),
        dryWet(inDryWet),
        biomeSize(inBiomeSize),
        biomeTerrainVariety(inBiomeTerrainVariety),
        mountainDensity(inMountainDensity),
        roughness(inRoughness),
        biomeBorderRoughness(inBiomeBorderRoughness),
        coastlineRoughness(inCoastlineRoughness),
        desertProbability(inDesertProbability),
        temperateForestProbability(inTemperateForestProbability),
        tropicalRainforestProbability(inTropicalRainforestProbability),
        savannaProbability(inSavannaProbability),
        temperateRainforestProbability(inTemperateRainforestProbability),
        borealForestProbability(inBorealForestProbability),
        grasslandProbability(inGrasslandProbability),
        woodlandProbability(inWoodlandProbability),
        tundraProbability(inTundraProbability),
        desertTexture(inDesertTexture),
        temperateForestTexture(inTemperateForestTexture),
        tropicalRainforestTexture(inTropicalRainforestTexture),
        savannaTexture(inSavannaTexture),
        temperateRainforestTexture(inTemperateRainforestTexture),
        borealForestTexture(inBorealForestTexture),
        grasslandTexture(inGrasslandTexture),
        woodlandTexture(inWoodlandTexture),
        tundraTexture(inTundraTexture)
    {};
    
    // Default constructor (TODO: set default values)
    Parameters() : Parameters(
        50, 10, 80, 70, 60, 48, 31, 30, 40, 20, 
        10, 70, 48, 20, 65, 43, 12, 69, 53, 34,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
       0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
       "grass.jpg","grass.jpg","grass.jpg","grass.jpg","grass.jpg",
       "grass.jpg","grass.jpg","grass.jpg","grass.jpg") {};

    void setDefaultValues() {
        maximumHeight = 50;
        seaLevel = 10;
        oceanCoverage = 80;
        landFragmentation = 70;
        continentSize = 60;
        riverWidth = 48;
        riverDepth = 31;
        riverFrequency = 30;
        riverLength = 40;
        riverVariety = 20;
        lakeFrequency = 10;
        treesDensity = 70;
        treeVariety = 48;
        treeHeight = 20;
        treeWidth = 65;
        treeBiomeExclusivity = 43;
        grassDensity = 12;
        grassVariety = 69;
        bushDensity = 53;
        bushVariety = 34;
        bushFrequency = 0;
        heightVariance = 0;
        warmCold = 0;
        dryWet = 0;
        biomeSize = 0;
        biomeTerrainVariety = 0;
        mountainDensity = 0;
        roughness = 0;
        biomeBorderRoughness = 0;
        coastlineRoughness = 0;
        desertProbability = 0;
        temperateForestProbability = 0;
        tropicalRainforestProbability = 0;
        savannaProbability = 0;
        temperateRainforestProbability = 0;
        borealForestProbability = 0;
        grasslandProbability = 0;
        woodlandProbability = 0;
        tundraProbability = 0;
        desertTexture = "grass.jpg";
        temperateForestTexture = "grass.jpg";
        tropicalRainforestTexture = "grass.jpg";
        savannaTexture = "grass.jpg";
        temperateRainforestTexture = "grass.jpg";
        borealForestTexture = "grass.jpg";
        grasslandTexture = "grass.jpg";
        woodlandTexture = "grass.jpg";
        tundraTexture = "grass.jpg";
    }

    bool saveToFile(string fileName, char filePathDelimitter) {
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

        // Write to file
        ofstream file(dataRoot + filePathDelimitter + "saved" + filePathDelimitter + fileName);
        if (!file) {
            cerr << "Error: Unable to open file for saving: " << fileName << endl;
            return 0;
        }
        file << jsonData.dump(4); // Pretty print with 4 spaces
        file.close();
        return 1;
    }

    void loadFromFile(string fileName, char filePathDelimitter) {
        string dataRoot = getenv("DATA_ROOT");
        // Read from file
        ifstream file(dataRoot + filePathDelimitter + "saved" + filePathDelimitter + fileName);
        if (!file) {
            cerr << "Error: Unable to open file for loading: " << "myFile" << endl;
            return;
        }
        json jsonData;

        file >> jsonData;
        file.close();

        // Set the parameters
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

    int& getMaximumHeight() { return maximumHeight; }
    int& getSeaLevel() { return seaLevel; }
    int& getOceanCoverage() { return oceanCoverage; }
    int& getLandFragmentation() { return landFragmentation; }
    int& getContinentSize() { return continentSize; }
    int& getRiverWidth() { return riverWidth; }

    int& getRiverDepth() { return riverDepth; }
    int& getRiverFrequency() { return riverFrequency; }
    int& getRiverLength() { return riverLength; }
    int& getRiverVariety() { return riverVariety; }
    int& getLakeFrequency() { return lakeFrequency; }
    int& getTreesDensity() { return treesDensity; }
    int& getTreeVariety() { return treeVariety; }
    int& getTreeHeight() { return treeHeight; }
    int& getTreeWidth() { return treeWidth; }
    int& getTreeBiomeExclusivity() { return treeBiomeExclusivity; }
    int& getGrassDensity() { return grassDensity; }
    int& getGrassVariety() { return grassVariety; }
    int& getBushDensity() { return bushDensity; }
    int& getBushVariety() { return bushVariety; }
    int& getBushFrequency() { return bushFrequency; }
    int& getHeightVariance() { return heightVariance; }

    int& getWarmCold() { return warmCold; }
    int& getDryWet() { return dryWet; }
    int& getBiomeSize() { return biomeSize; }
    int& getBiomeTerrainVariety() { return biomeTerrainVariety; }
    int& getMountainDensity() { return mountainDensity; }
    int& getRoughness() { return roughness; }
    int& getBiomeBorderRoughness() { return biomeBorderRoughness; }
    int& getCoastlineRoughness() { return coastlineRoughness; }

    int& getDesertProbability() { return desertProbability; }
    int& getTemperateForestProbability() { return temperateForestProbability; }
    int& getTropicalRainforestProbability() { return tropicalRainforestProbability; }
    int& getSavannaProbability() { return savannaProbability; }
    int& getTemperateRainforestProbability() { return temperateRainforestProbability; }
    int& getBorealForestProbability() { return borealForestProbability; }
    int& getGrasslandProbability() { return grasslandProbability; }
    int& getWoodlandProbability() { return woodlandProbability; }
    int& getTundraProbability() { return tundraProbability; }

    string getDesertTexture() { return desertTexture; }
    string getTemperateForestTexture() { return temperateForestTexture; }
    string getTropicalRainforestTexture() { return tropicalRainforestTexture; }
    string getSavannaTexture() { return savannaTexture; }
    string getTemperateRainforestTexture() { return temperateRainforestTexture; }
    string getBorealForestTexture() { return borealForestTexture; }
    string getGrasslandTexture() { return grasslandTexture; }
    string getWoodlandTexture() { return woodlandTexture; }
    string getTundraTexture() { return tundraTexture; }

    void setMaximumHeight(int inMaximumHeight) { maximumHeight = inMaximumHeight; }
    void setSeaLevel(int inSeaLevel) { seaLevel = inSeaLevel; }
    void setOceanCoverage(int inOceanCoverage) { oceanCoverage = inOceanCoverage; }
    void setLandFragmentation(int inLandFragmentation) { landFragmentation = inLandFragmentation; }
    void setContinentSize(int inContinentSize) { continentSize = inContinentSize; }
    void setRiverWidth(int inRiverWidth) { riverWidth = inRiverWidth; }
    void setRiverDepth(int inRiverDepth) { riverDepth = inRiverDepth; }
    void setRiverFrequency(int inRiverFrequency) { riverFrequency = inRiverFrequency; }
    void setRiverLength(int inRiverLength) { riverLength = inRiverLength; }
    void setRiverVariety(int inRiverVariety) { riverVariety = inRiverVariety; }
    void setLakeFrequency(int inLakeFrequency) { lakeFrequency = inLakeFrequency; }
    void setTreesDensity(int inTreesDensity) { treesDensity = inTreesDensity; }
    void setTreeVariety(int inTreeVariety) { treeVariety = inTreeVariety; }
    void setTreeHeight(int inTreeHeight) { treeHeight = inTreeHeight; }
    void setTreeWidth(int inTreeWidth) { treeWidth = inTreeWidth; }
    void setTreeBiomeExclusivity(int inTreeBiomeExclusivity) { treeBiomeExclusivity = inTreeBiomeExclusivity; }
    void setGrassDensity(int inGrassDensity) { grassDensity = inGrassDensity; }
    void setGrassVariety(int inGrassVariety) { grassVariety = inGrassVariety; }
    void setBushDensity(int inBushDensity) { bushDensity = inBushDensity; }
    void setBushVariety(int inBushVariety) { bushVariety = inBushVariety; }
    void setBushFrequency(int inBushFrequency) { bushFrequency = inBushFrequency; }
    void setHeightVariance(int inHeightVariance) { heightVariance = inHeightVariance; }

    void setWarmCold(int inWarmCold) { warmCold = inWarmCold; }
    void setDryWet(int inDryWet) { dryWet = inDryWet; }
    void setBiomeSize(int inBiomeSize) { biomeSize = inBiomeSize; }
    void setBiomeTerrainVariety(int inBiomeTerrainVariety) { biomeTerrainVariety = inBiomeTerrainVariety; }
    void setMountainDensity(int inMountainDensity) { mountainDensity = inMountainDensity; }
    void setRoughness(int inRoughness) { roughness = inRoughness; }
    void setBiomeBorderRoughness(int inBiomeBorderRoughness) { biomeBorderRoughness = inBiomeBorderRoughness; }
    void setCoastlineRoughness(int inCoastlineRoughness) { coastlineRoughness = inCoastlineRoughness; }

    void setDesertProbability(int inDesertProbability) { desertProbability = inDesertProbability; }
    void setTemperateForestProbability(int inTemperateForestProbability) { temperateForestProbability = inTemperateForestProbability; }
    void setTropicalRainforestProbability(int inTropicalRainforestProbability) { tropicalRainforestProbability = inTropicalRainforestProbability; }
    void setSavannaProbability(int inSavannaProbability) { savannaProbability = inSavannaProbability; }
    void setTemperateRainforestProbability(int inTemperateRainforestProbability) { temperateRainforestProbability = inTemperateRainforestProbability; }
    void setBorealForestProbability(int inBorealForestProbability) { borealForestProbability = inBorealForestProbability; }
    void setGrasslandProbability(int inGrasslandProbability) { grasslandProbability = inGrasslandProbability; }
    void setWoodlandProbability(int inWoodlandProbability) { woodlandProbability = inWoodlandProbability; }
    void setTundraProbability(int inTundraProbability) { tundraProbability = inTundraProbability; }

    void setDesertTexture(string inDesertTexture) { desertTexture = inDesertTexture; }
    void setTemperateForestTexture(string inTemperateForestTexture) { temperateForestTexture = inTemperateForestTexture; }
    void setTropicalRainforestTexture(string inTropicalRainforestTexture) { tropicalRainforestTexture = inTropicalRainforestTexture; }
    void setSavannaTexture(string inSavannaTexture) { savannaTexture = inSavannaTexture; }
    void setTemperateRainforestTexture(string inTemperateRainforestTexture) { temperateRainforestTexture = inTemperateRainforestTexture; }
    void setBorealForestTexture(string inBorealForestTexture) { borealForestTexture = inBorealForestTexture; }
    void setGrasslandTexture(string inGrasslandTexture) { grasslandTexture = inGrasslandTexture; }
    void setWoodlandTexture(string inWoodlandTexture) { woodlandTexture = inWoodlandTexture; }
    void setTundraTexture(string inTundraTexture) { tundraTexture = inTundraTexture; }

};

#endif
