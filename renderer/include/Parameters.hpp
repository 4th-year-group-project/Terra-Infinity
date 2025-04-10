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
    long seed;
    int maximumHeight;
    int seaLevel;
    int oceanCoverage;
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

    
    string temperateRainforestTexture1;
    string temperateRainforestTexture2;
    string temperateRainforestTexture3;
    string temperateRainforestTexture4;
    string borealForestTexture1;
    string borealForestTexture2;
    string borealForestTexture3;
    string borealForestTexture4;
    string grasslandTexture1;
    string grasslandTexture2;
    string grasslandTexture3;
    string grasslandTexture4;
    string tundraTexture1;
    string tundraTexture2;
    string tundraTexture3;
    string tundraTexture4;
    string savannaTexture1;
    string savannaTexture2;
    string savannaTexture3;
    string savannaTexture4;
    string woodlandTexture1;
    string woodlandTexture2;
    string woodlandTexture3;
    string woodlandTexture4;
    string tropicalRainforestTexture1;
    string tropicalRainforestTexture2;
    string tropicalRainforestTexture3;
    string tropicalRainforestTexture4;
    string temperateForestTexture1;
    string temperateForestTexture2;
    string temperateForestTexture3;
    string temperateForestTexture4;
    string desertTexture1;
    string desertTexture2;
    string desertTexture3;
    string desertTexture4;

public:
    // Constructor with parameters
    Parameters(
        long inSeed,
        int inMaximumHeight,
        int inSeaLevel,
        int inOceanCoverage,
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
        string inTemperateRainforestTexture1,
        string inTemperateRainforestTexture2,
        string inTemperateRainforestTexture3,
        string inTemperateRainforestTexture4,
        string inBorealForestTexture1,
        string inBorealForestTexture2,
        string inBorealForestTexture3,
        string inBorealForestTexture4,
        string inGrasslandTexture1,
        string inGrasslandTexture2,
        string inGrasslandTexture3,
        string inGrasslandTexture4,
        string inTundraTexture1,
        string inTundraTexture2,
        string inTundraTexture3,
        string inTundraTexture4,
        string inSavannaTexture1,
        string inSavannaTexture2,
        string inSavannaTexture3,
        string inSavannaTexture4,
        string inWoodlandTexture1,
        string inWoodlandTexture2,
        string inWoodlandTexture3,
        string inWoodlandTexture4,
        string inTropicalRainforestTexture1,
        string inTropicalRainforestTexture2,
        string inTropicalRainforestTexture3,
        string inTropicalRainforestTexture4,
        string inTemperateForestTexture1,
        string inTemperateForestTexture2,
        string inTemperateForestTexture3,
        string inTemperateForestTexture4,
        string inDesertTexture1,
        string inDesertTexture2,
        string inDesertTexture3,
        string inDesertTexture4
    ) :
        seed(inSeed),
        maximumHeight(inMaximumHeight),
        seaLevel(inSeaLevel),
        oceanCoverage(inOceanCoverage),
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
        temperateRainforestTexture1(inTemperateRainforestTexture1),
        temperateRainforestTexture2(inTemperateRainforestTexture2),
        temperateRainforestTexture3(inTemperateRainforestTexture3),
        temperateRainforestTexture4(inTemperateRainforestTexture4),
        borealForestTexture1(inBorealForestTexture1),
        borealForestTexture2(inBorealForestTexture2),
        borealForestTexture3(inBorealForestTexture3),
        borealForestTexture4(inBorealForestTexture4),
        grasslandTexture1(inGrasslandTexture1),
        grasslandTexture2(inGrasslandTexture2),
        grasslandTexture3(inGrasslandTexture3),
        grasslandTexture4(inGrasslandTexture4),
        tundraTexture1(inTundraTexture1),
        tundraTexture2(inTundraTexture2),
        tundraTexture3(inTundraTexture3),
        tundraTexture4(inTundraTexture4),
        savannaTexture1(inSavannaTexture1),
        savannaTexture2(inSavannaTexture2),
        savannaTexture3(inSavannaTexture3),
        savannaTexture4(inSavannaTexture4),
        woodlandTexture1(inWoodlandTexture1),
        woodlandTexture2(inWoodlandTexture2),
        woodlandTexture3(inWoodlandTexture3),
        woodlandTexture4(inWoodlandTexture4),
        tropicalRainforestTexture1(inTropicalRainforestTexture1),
        tropicalRainforestTexture2(inTropicalRainforestTexture2),
        tropicalRainforestTexture3(inTropicalRainforestTexture3),
        tropicalRainforestTexture4(inTropicalRainforestTexture4),
        temperateForestTexture1(inTemperateForestTexture1),
        temperateForestTexture2(inTemperateForestTexture2),
        temperateForestTexture3(inTemperateForestTexture3),
        temperateForestTexture4(inTemperateForestTexture4),
        desertTexture1(inDesertTexture1),
        desertTexture2(inDesertTexture2),
        desertTexture3(inDesertTexture3),
        desertTexture4(inDesertTexture4)
    {};

    // Default constructor
    Parameters();

    void setDefaultValues();

    bool saveToFile(string fileName, char filePathDelimitter);
    void loadFromFile(string fileName, char filePathDelimitter);

    long& getSeed() { return seed; }
    void setSeed(long inSeed) { seed = inSeed; }

    int& getMaximumHeight() { return maximumHeight; }
    int& getSeaLevel() { return seaLevel; }
    int& getOceanCoverage() { return oceanCoverage; }
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

    string getDesertTexture1() { return desertTexture1; }
    string getDesertTexture2() { return desertTexture2; }
    string getDesertTexture3() { return desertTexture3; }
    string getDesertTexture4() { return desertTexture4; }
    string getTemperateForestTexture1() { return temperateForestTexture1; }
    string getTemperateForestTexture2() { return temperateForestTexture2; }
    string getTemperateForestTexture3() { return temperateForestTexture3; }
    string getTemperateForestTexture4() { return temperateForestTexture4; }
    string getTropicalRainforestTexture1() { return tropicalRainforestTexture1; }
    string getTropicalRainforestTexture2() { return tropicalRainforestTexture2; }
    string getTropicalRainforestTexture3() { return tropicalRainforestTexture3; }
    string getTropicalRainforestTexture4() { return tropicalRainforestTexture4; }
    string getSavannaTexture1() { return savannaTexture1; }
    string getSavannaTexture2() { return savannaTexture2; }
    string getSavannaTexture3() { return savannaTexture3; }
    string getSavannaTexture4() { return savannaTexture4; }
    string getTemperateRainforestTexture1() { return temperateRainforestTexture1; }
    string getTemperateRainforestTexture2() { return temperateRainforestTexture2; }
    string getTemperateRainforestTexture3() { return temperateRainforestTexture3; }
    string getTemperateRainforestTexture4() { return temperateRainforestTexture4; }
    string getBorealForestTexture1() { return borealForestTexture1; }
    string getBorealForestTexture2() { return borealForestTexture2; }
    string getBorealForestTexture3() { return borealForestTexture3; }
    string getBorealForestTexture4() { return borealForestTexture4; }
    string getGrasslandTexture1() { return grasslandTexture1; }
    string getGrasslandTexture2() { return grasslandTexture2; }
    string getGrasslandTexture3() { return grasslandTexture3; }
    string getGrasslandTexture4() { return grasslandTexture4; }
    string getWoodlandTexture1() { return woodlandTexture1; }
    string getWoodlandTexture2() { return woodlandTexture2; }
    string getWoodlandTexture3() { return woodlandTexture3; }
    string getWoodlandTexture4() { return woodlandTexture4; }
    string getTundraTexture1() { return tundraTexture1; }
    string getTundraTexture2() { return tundraTexture2; }
    string getTundraTexture3() { return tundraTexture3; }
    string getTundraTexture4() { return tundraTexture4; }

    void setMaximumHeight(int inMaximumHeight) { maximumHeight = inMaximumHeight; }
    void setSeaLevel(int inSeaLevel) { seaLevel = inSeaLevel; }
    void setOceanCoverage(int inOceanCoverage) { oceanCoverage = inOceanCoverage; }
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

    void setDesertTexture1(string inDesertTexture1) { desertTexture1 = inDesertTexture1; }
    void setDesertTexture2(string inDesertTexture2) { desertTexture2 = inDesertTexture2; }
    void setDesertTexture3(string inDesertTexture3) { desertTexture3 = inDesertTexture3; }
    void setDesertTexture4(string inDesertTexture4) { desertTexture4 = inDesertTexture4; }
    void setTemperateForestTexture1(string inTemperateForestTexture1) { temperateForestTexture1 = inTemperateForestTexture1; }
    void setTemperateForestTexture2(string inTemperateForestTexture2) { temperateForestTexture2 = inTemperateForestTexture2; }
    void setTemperateForestTexture3(string inTemperateForestTexture3) { temperateForestTexture3 = inTemperateForestTexture3; }
    void setTemperateForestTexture4(string inTemperateForestTexture4) { temperateForestTexture4 = inTemperateForestTexture4; }
    void setTropicalRainforestTexture1(string inTropicalRainforestTexture1) { tropicalRainforestTexture1 = inTropicalRainforestTexture1; }
    void setTropicalRainforestTexture2(string inTropicalRainforestTexture2) { tropicalRainforestTexture2 = inTropicalRainforestTexture2; }
    void setTropicalRainforestTexture3(string inTropicalRainforestTexture3) { tropicalRainforestTexture3 = inTropicalRainforestTexture3; }
    void setTropicalRainforestTexture4(string inTropicalRainforestTexture4) { tropicalRainforestTexture4 = inTropicalRainforestTexture4; }
    void setSavannaTexture1(string inSavannaTexture1) { savannaTexture1 = inSavannaTexture1; }
    void setSavannaTexture2(string inSavannaTexture2) { savannaTexture2 = inSavannaTexture2; }
    void setSavannaTexture3(string inSavannaTexture3) { savannaTexture3 = inSavannaTexture3; }
    void setSavannaTexture4(string inSavannaTexture4) { savannaTexture4 = inSavannaTexture4; }
    void setTemperateRainforestTexture1(string inTemperateRainforestTexture1) { temperateRainforestTexture1 = inTemperateRainforestTexture1; }
    void setTemperateRainforestTexture2(string inTemperateRainforestTexture2) { temperateRainforestTexture2 = inTemperateRainforestTexture2; }
    void setTemperateRainforestTexture3(string inTemperateRainforestTexture3) { temperateRainforestTexture3 = inTemperateRainforestTexture3; }
    void setTemperateRainforestTexture4(string inTemperateRainforestTexture4) { temperateRainforestTexture4 = inTemperateRainforestTexture4; }
    void setBorealForestTexture1(string inBorealForestTexture1) { borealForestTexture1 = inBorealForestTexture1; }
    void setBorealForestTexture2(string inBorealForestTexture2) { borealForestTexture2 = inBorealForestTexture2; }
    void setBorealForestTexture3(string inBorealForestTexture3) { borealForestTexture3 = inBorealForestTexture3; }
    void setBorealForestTexture4(string inBorealForestTexture4) { borealForestTexture4 = inBorealForestTexture4; }
    void setGrasslandTexture1(string inGrasslandTexture1) { grasslandTexture1 = inGrasslandTexture1; }
    void setGrasslandTexture2(string inGrasslandTexture2) { grasslandTexture2 = inGrasslandTexture2; }
    void setGrasslandTexture3(string inGrasslandTexture3) { grasslandTexture3 = inGrasslandTexture3; }
    void setGrasslandTexture4(string inGrasslandTexture4) { grasslandTexture4 = inGrasslandTexture4; }
    void setWoodlandTexture1(string inWoodlandTexture1) { woodlandTexture1 = inWoodlandTexture1; }
    void setWoodlandTexture2(string inWoodlandTexture2) { woodlandTexture2 = inWoodlandTexture2; }
    void setWoodlandTexture3(string inWoodlandTexture3) { woodlandTexture3 = inWoodlandTexture3; }
    void setWoodlandTexture4(string inWoodlandTexture4) { woodlandTexture4 = inWoodlandTexture4; }
    void setTundraTexture1(string inTundraTexture1) { tundraTexture1 = inTundraTexture1; }
    void setTundraTexture2(string inTundraTexture2) { tundraTexture2 = inTundraTexture2; }
    void setTundraTexture3(string inTundraTexture3) { tundraTexture3 = inTundraTexture3; }
    void setTundraTexture4(string inTundraTexture4) { tundraTexture4 = inTundraTexture4; }

};

#endif
