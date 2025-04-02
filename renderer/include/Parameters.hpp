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
    
    // Default constructor
    Parameters();

    void setDefaultValues();

    bool saveToFile(string fileName, char filePathDelimitter);
    void loadFromFile(string fileName, char filePathDelimitter);

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
