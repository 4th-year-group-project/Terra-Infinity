#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include <string>

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
        int inTundraProbability
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
        tundraProbability(inTundraProbability)
    {};
    
    // Default constructor (delegating to parameterized constructor with default values)
    Parameters() : Parameters(
        50, 50, 50, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
       0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) {};

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

};

#endif
