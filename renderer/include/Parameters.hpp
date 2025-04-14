#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

/*
    This class will hold all of the values of parameters the user has chosen that will be used in generating terrain and texturing.
*/
class Parameters
{
private:
    // Global parameters
    long seed;
    int maxHeight;
    int oceanCoverage;
    int continentSize;
    int roughness;
    int mountainousness;
    int coastlineRoughness;
    int biomeSize;
    int warmth;
    int wetness;
    int riverFrequency;
    int riverWidth;
    int riverDepth;
    int riverMeandering;
    int riverSmoothness;
    int treesDensity;

    // Boreal forest parameters
    bool borealForestSelected;
    int borealForestPlainsMaxHeight;
    int borealForestPlainsOccurrenceProbability;
    int borealForestPlainsEvenness;
    int borealForestPlainsTreeDensity;
    int borealForestHillsMaxHeight;
    int borealForestHillsOccurrenceProbability;
    int borealForestHillsBumpiness;
    int borealForestHillsTreeDensity;
    int borealForestMountainsMaxHeight;
    int borealForestMountainsOccurrenceProbability;
    int borealForestMountainsRuggedness;
    int borealForestMountainsTreeDensity;

    // Grassland
    bool grasslandSelected;
    int grasslandPlainsMaxHeight;
    int grasslandPlainsOccurrenceProbability;
    int grasslandPlainsEvenness;
    int grasslandPlainsTreeDensity;
    int grasslandHillsMaxHeight;
    int grasslandHillsOccurrenceProbability;
    int grasslandHillsBumpiness;
    int grasslandHillsTreeDensity;
    int grasslandRockyFieldsMaxHeight;
    int grasslandRockyFieldsOccurrenceProbability;
    int grasslandRockyFieldsRockiness;
    int grasslandRockyFieldsTreeDensity;
    int grasslandTerracedFieldsMaxHeight;
    int grasslandTerracedFieldsOccurrenceProbability;
    int grasslandTerracedFieldsSize;
    int grasslandTerracedFieldsTreeDensity;
    int grasslandTerracedFieldsSmoothness;
    int grasslandTerracedFieldsNumberOfTerraces;

    // Tundra
    bool tundraSelected;
    int tundraPlainsMaxHeight;
    int tundraPlainsOccurrenceProbability;
    int tundraPlainsEvenness;
    int tundraPlainsTreeDensity;
    int tundraBluntMountainsMaxHeight;
    int tundraBluntMountainsOccurrenceProbability;
    int tundraBluntMountainsRuggedness;
    int tundraBluntMountainsTreeDensity;
    int tundraPointyMountainsMaxHeight;
    int tundraPointyMountainsOccurrenceProbability;
    int tundraPointyMountainsSteepness;
    int tundraPointyMountainsFrequency;
    int tundraPointyMountainsTreeDensity;

    // Savanna
    bool savannaSelected;
    int savannaPlainsMaxHeight;
    int savannaPlainsOccurrenceProbability;
    int savannaPlainsEvenness;
    int savannaPlainsTreeDensity;
    int savannaMountainsMaxHeight;
    int savannaMountainsOccurrenceProbability;
    int savannaMountainsRuggedness;
    int savannaMountainsTreeDensity;

    // Woodland
    bool woodlandSelected;
    int woodlandHillsMaxHeight;
    int woodlandHillsOccurrenceProbability;
    int woodlandHillsBumpiness;
    int woodlandHillsTreeDensity;

    // Tropical Rainforest
    bool tropicalRainforestSelected;
    int tropicalRainforestPlainsMaxHeight;
    int tropicalRainforestPlainsOccurrenceProbability;
    int tropicalRainforestPlainsEvenness;
    int tropicalRainforestPlainsTreeDensity;
    int tropicalRainforestMountainsMaxHeight;
    int tropicalRainforestMountainsOccurrenceProbability;
    int tropicalRainforestMountainsRuggedness;
    int tropicalRainforestMountainsTreeDensity;
    int tropicalRainforestHillsMaxHeight;
    int tropicalRainforestHillsOccurrenceProbability;
    int tropicalRainforestHillsBumpiness;
    int tropicalRainforestHillsTreeDensity;
    int tropicalRainforestVolcanoesMaxHeight;
    int tropicalRainforestVolcanoesOccurrenceProbability;
    int tropicalRainforestVolcanoesSize;
    int tropicalRainforestVolcanoesTreeDensity;
    int tropicalRainforestVolcanoesThickness;
    int tropicalRainforestVolcanoesDensity;

    // Temperate Rainforest parameters
    bool temperateRainforestSelected;
    int temperateRainforestHillsMaxHeight;
    int temperateRainforestHillsOccurrenceProbability;
    int temperateRainforestHillsBumpiness;
    int temperateRainforestHillsTreeDensity;
    int temperateRainforestMountainsMaxHeight;
    int temperateRainforestMountainsOccurrenceProbability;
    int temperateRainforestMountainsRuggedness;
    int temperateRainforestMountainsTreeDensity;
    int temperateRainforestSwampMaxHeight;
    int temperateRainforestSwampOccurrenceProbability;
    int temperateRainforestSwampWetness;
    int temperateRainforestSwampTreeDensity;

    // Temperate Seasonal Forest parameters
    bool temperateSeasonalForestSelected;
    int temperateSeasonalForestHillsMaxHeight;
    int temperateSeasonalForestHillsOccurrenceProbability;
    int temperateSeasonalForestHillsBumpiness;
    int temperateSeasonalForestHillsTreeDensity;
    int temperateSeasonalForestHillsAutumnalOccurrence;
    int temperateSeasonalForestMountainsMaxHeight;
    int temperateSeasonalForestMountainsOccurrenceProbability;
    int temperateSeasonalForestMountainsRuggedness;
    int temperateSeasonalForestMountainsTreeDensity;
    int temperateSeasonalForestMountainsAutumnalOccurrence;

    // Subtropical Desert parameters
    bool subtropicalDesertSelected;
    int subtropicalDesertDunesMaxHeight;
    int subtropicalDesertDunesOccurrenceProbability;
    int subtropicalDesertDunesSize;
    int subtropicalDesertDunesTreeDensity;
    int subtropicalDesertDunesDuneFrequency;
    int subtropicalDesertDunesDuneWaviness;
    int subtropicalDesertDunesBumpiness;
    int subtropicalDesertMesasMaxHeight;
    int subtropicalDesertMesasOccurrenceProbability;
    int subtropicalDesertMesasSize;
    int subtropicalDesertMesasTreeDensity;
    int subtropicalDesertMesasNumberOfTerraces;
    int subtropicalDesertMesasSteepness;
    int subtropicalDesertRavinesMaxHeight;
    int subtropicalDesertRavinesOccurrenceProbability;
    int subtropicalDesertRavinesDensity;
    int subtropicalDesertRavinesTreeDensity;
    int subtropicalDesertRavinesRavineWidth;
    int subtropicalDesertRavinesSmoothness;
    int subtropicalDesertRavinesSteepness;
    int subtropicalDesertOasisMaxHeight;
    int subtropicalDesertOasisOccurrenceProbability;
    int subtropicalDesertOasisSize;
    int subtropicalDesertOasisFlatness;
    int subtropicalDesertOasisTreeDensity;
    int subtropicalDesertOasisDuneFrequency;
    int subtropicalDesertCrackedMaxHeight;
    int subtropicalDesertCrackedOccurrenceProbability;
    int subtropicalDesertCrackedSize;
    int subtropicalDesertCrackedFlatness;
    int subtropicalDesertCrackedTreeDensity;

    // Ocean parameters
    int oceanFlatSeabedMaxHeight;
    int oceanFlatSeabedEvenness;
    int oceanFlatSeabedOccurrenceProbability;
    int oceanVolcanicIslandsMaxHeight;
    int oceanVolcanicIslandsOccurrenceProbability;
    int oceanVolcanicIslandsSize;
    int oceanVolcanicIslandsThickness;
    int oceanVolcanicIslandsDensity;
    int oceanWaterStacksMaxHeight;
    int oceanWaterStacksOccurrenceProbability;
    int oceanWaterStacksSize;
    int oceanTrenchesDensity;
    int oceanTrenchesOccurrenceProbability;
    int oceanTrenchesTrenchWidth;
    int oceanTrenchesSmoothness;

    // Textures
    // Boreal
    string borealTextureLow;
    string borealTextureMidFlat;
    string borealTextureMidSteep;
    string borealTextureHigh;

    // Grassy
    string grassyTextureLow;
    string grassyTextureMidFlat;
    string grassyTextureMidSteep;
    string grassyTextureHigh;

    // GrassyStone
    string grassyStoneTextureLow;
    string grassyStoneTextureMidFlat;
    string grassyStoneTextureMidSteep;
    string grassyStoneTextureHigh;

    // Snowy
    string snowyTextureLow;
    string snowyTextureMidFlat;
    string snowyTextureMidSteep;
    string snowyTextureHigh;

    // Icy
    string icyTextureLow;
    string icyTextureMidFlat;
    string icyTextureMidSteep;
    string icyTextureHigh;

    // Savanna
    string savannaTextureLow;
    string savannaTextureMidFlat;
    string savannaTextureMidSteep;
    string savannaTextureHigh;

    // Forest
    string woodlandTextureLow;
    string woodlandTextureMidFlat;
    string woodlandTextureMidSteep;
    string woodlandTextureHigh;

    // Jungle
    string jungleTextureLow;
    string jungleTextureMidFlat;
    string jungleTextureMidSteep;
    string jungleTextureHigh;

    // Jungle Mountains
    string jungleMountainsTextureLow;
    string jungleMountainsTextureMidFlat;
    string jungleMountainsTextureMidSteep;
    string jungleMountainsTextureHigh;

    // Volcanic
    string volcanicTextureLow;
    string volcanicTextureMidFlat;
    string volcanicTextureMidSteep;
    string volcanicTextureHigh;

    // Temperate
    string temperateTextureLow;
    string temperateTextureMidFlat;
    string temperateTextureMidSteep;
    string temperateTextureHigh;

    // Swamp
    string swampTextureLow;
    string swampTextureMidFlat;
    string swampTextureMidSteep;
    string swampTextureHigh;

    // Seasonal forest
    string seasonalForestTextureLow;
    string seasonalForestTextureMidFlat;
    string seasonalForestTextureMidSteep;
    string seasonalForestTextureHigh;

    // Autumn
    string autumnTextureLow;
    string autumnTextureMidFlat;
    string autumnTextureMidSteep;
    string autumnTextureHigh;

    // Mesa
    string mesaTextureLow;
    string mesaTextureMidFlat;
    string mesaTextureMidSteep;
    string mesaTextureHigh;

    // Hot desert
    string hotDesertTextureLow;
    string hotDesertTextureMidFlat;
    string hotDesertTextureMidSteep;
    string hotDesertTextureHigh;

    // Dusty
    string dustyTextureLow;
    string dustyTextureMidFlat;
    string dustyTextureMidSteep;
    string dustyTextureHigh;

    // Badlands
    string badlandsTextureLow;
    string badlandsTextureMidFlat;
    string badlandsTextureMidSteep;
    string badlandsTextureHigh;

    // Oasis
    string oasisTextureLow;
    string oasisTextureMidFlat;
    string oasisTextureMidSteep;
    string oasisTextureHigh;

    // Ocean
    string oceanTextureLow;
    string oceanTextureMidFlat;
    string oceanTextureMidSteep;
    string oceanTextureHigh;

    // Cliffs
    string cliffsTextureLow;
    string cliffsTextureMidFlat;
    string cliffsTextureMidSteep;
    string cliffsTextureHigh;

    void setRandomSeed(string worldName);

public:
    // Default constructor
    Parameters();

    // Functions
    bool saveToFile(string fileName, char filePathDelimitter);
    void loadFromFile(string fileName, char filePathDelimitter);
    string findTextureFilePath(string textureName, char filePathDelimitter, string type);
    void setDefaultValues(string worldName);

    // Getters and setters for parameters
    long getSeed() { return seed; }
    void setSeed(long inSeed) { seed = inSeed; }
    int& getMaxHeight() { return maxHeight; }
    void setMaxHeight(int inMaxHeight) { maxHeight = inMaxHeight; }
    int& getOceanCoverage() { return oceanCoverage; }
    void setOceanCoverage(int inOceanCoverage) { oceanCoverage = inOceanCoverage; }
    int& getContinentSize() { return continentSize; }
    void setContinentSize(int inContinentSize) { continentSize = inContinentSize; }
    int& getRoughness() { return roughness; }
    void setRoughness(int inRoughness) { roughness = inRoughness; }
    int& getMountainousness() { return mountainousness; }
    void setMountainousness(int inMountainousness) { mountainousness = inMountainousness; }
    int& getCoastlineRoughness() { return coastlineRoughness; }
    void setCoastlineRoughness(int inCoastlineRoughness) { coastlineRoughness = inCoastlineRoughness; }
    int& getBiomeSize() { return biomeSize; }
    void setBiomeSize(int inBiomeSize) { biomeSize = inBiomeSize; }
    int& getWarmth() { return warmth; }
    void setWarmth(int inWarmth) { warmth = inWarmth; }
    int& getWetness() { return wetness; }
    void setWetness(int inWetness) { wetness = inWetness; }
    int& getRiverFrequency() { return riverFrequency; }
    void setRiverFrequency(int inRiverFrequency) { riverFrequency = inRiverFrequency; }
    int& getRiverWidth() { return riverWidth; }
    void setRiverWidth(int inRiverWidth) { riverWidth = inRiverWidth; }
    int& getRiverDepth() { return riverDepth; }
    void setRiverDepth(int inRiverDepth) { riverDepth = inRiverDepth; }
    int& getRiverMeandering() { return riverMeandering; }
    void setRiverMeandering(int inRiverMeandering) { riverMeandering = inRiverMeandering; }
    int& getRiverSmoothness() { return riverSmoothness; }
    void setRiverSmoothness(int inRiverSmoothness) { riverSmoothness = inRiverSmoothness; }
    int& getTreesDensity() { return treesDensity; }
    void setTreesDensity(int inTreesDensity) { treesDensity = inTreesDensity; }

    // Boreal forest getters and setters
    bool& getBorealForestSelected() { return borealForestSelected; }
    void setBorealForestSelected(bool inBorealForestSelected) { borealForestSelected = inBorealForestSelected; }
    int& getBorealForestPlainsMaxHeight() { return borealForestPlainsMaxHeight; }
    void setBorealForestPlainsMaxHeight(int inBorealForestPlainsMaxHeight) { borealForestPlainsMaxHeight = inBorealForestPlainsMaxHeight; }
    int& getBorealForestPlainsOccurrenceProbability() { return borealForestPlainsOccurrenceProbability; }
    void setBorealForestPlainsOccurrenceProbability(int inBorealForestPlainsOccurrenceProbability) { borealForestPlainsOccurrenceProbability = inBorealForestPlainsOccurrenceProbability; }
    int& getBorealForestPlainsEvenness() { return borealForestPlainsEvenness; }
    void setBorealForestPlainsEvenness(int inBorealForestPlainsEvenness) { borealForestPlainsEvenness = inBorealForestPlainsEvenness; }
    int& getBorealForestPlainsTreeDensity() { return borealForestPlainsTreeDensity; }
    void setBorealForestPlainsTreeDensity(int inBorealForestPlainsTreeDensity) { borealForestPlainsTreeDensity = inBorealForestPlainsTreeDensity; }
    int& getBorealForestHillsMaxHeight() { return borealForestHillsMaxHeight; }
    void setBorealForestHillsMaxHeight(int inBorealForestHillsMaxHeight) { borealForestHillsMaxHeight = inBorealForestHillsMaxHeight; }
    int& getBorealForestHillsOccurrenceProbability() { return borealForestHillsOccurrenceProbability; } 
    void setBorealForestHillsOccurrenceProbability(int inBorealForestHillsOccurrenceProbability) { borealForestHillsOccurrenceProbability = inBorealForestHillsOccurrenceProbability; }
    int& getBorealForestHillsBumpiness() { return borealForestHillsBumpiness; }
    void setBorealForestHillsBumpiness(int inBorealForestHillsBumpiness) { borealForestHillsBumpiness = inBorealForestHillsBumpiness; }
    int& getBorealForestHillsTreeDensity() { return borealForestHillsTreeDensity; }
    void setBorealForestHillsTreeDensity(int inBorealForestHillsTreeDensity) { borealForestHillsTreeDensity = inBorealForestHillsTreeDensity; }
    int& getBorealForestMountainsMaxHeight() { return borealForestMountainsMaxHeight; }
    void setBorealForestMountainsMaxHeight(int inBorealForestMountainsMaxHeight) { borealForestMountainsMaxHeight = inBorealForestMountainsMaxHeight; }
    int& getBorealForestMountainsOccurrenceProbability() { return borealForestMountainsOccurrenceProbability; }
    void setBorealForestMountainsOccurrenceProbability(int inBorealForestMountainsOccurrenceProbability) { borealForestMountainsOccurrenceProbability = inBorealForestMountainsOccurrenceProbability; }
    int& getBorealForestMountainsRuggedness() { return borealForestMountainsRuggedness; }
    void setBorealForestMountainsRuggedness(int inBorealForestMountainsRuggedness) { borealForestMountainsRuggedness = inBorealForestMountainsRuggedness; }
    int& getBorealForestMountainsTreeDensity() { return borealForestMountainsTreeDensity; }
    void setBorealForestMountainsTreeDensity(int inBorealForestMountainsTreeDensity) { borealForestMountainsTreeDensity = inBorealForestMountainsTreeDensity; }

    // Grassland getters and setters
    bool& getGrasslandSelected() { return grasslandSelected; }
    void setGrasslandSelected(bool inGrasslandSelected) { grasslandSelected = inGrasslandSelected; }
    int& getGrasslandPlainsMaxHeight() { return grasslandPlainsMaxHeight; }
    void setGrasslandPlainsMaxHeight(int inGrasslandPlainsMaxHeight) { grasslandPlainsMaxHeight = inGrasslandPlainsMaxHeight; }
    int& getGrasslandPlainsOccurrenceProbability() { return grasslandPlainsOccurrenceProbability; }
    void setGrasslandPlainsOccurrenceProbability(int inGrasslandPlainsOccurrenceProbability) { grasslandPlainsOccurrenceProbability = inGrasslandPlainsOccurrenceProbability; }
    int& getGrasslandPlainsEvenness() { return grasslandPlainsEvenness; }
    void setGrasslandPlainsEvenness(int inGrasslandPlainsEvenness) { grasslandPlainsEvenness = inGrasslandPlainsEvenness; }
    int& getGrasslandPlainsTreeDensity() { return grasslandPlainsTreeDensity; }
    void setGrasslandPlainsTreeDensity(int inGrasslandPlainsTreeDensity) { grasslandPlainsTreeDensity = inGrasslandPlainsTreeDensity; }
    int& getGrasslandHillsMaxHeight() { return grasslandHillsMaxHeight; }
    void setGrasslandHillsMaxHeight(int inGrasslandHillsMaxHeight) { grasslandHillsMaxHeight = inGrasslandHillsMaxHeight; }
    int& getGrasslandHillsOccurrenceProbability() { return grasslandHillsOccurrenceProbability; }
    void setGrasslandHillsOccurrenceProbability(int inGrasslandHillsOccurrenceProbability) { grasslandHillsOccurrenceProbability = inGrasslandHillsOccurrenceProbability; }
    int& getGrasslandHillsBumpiness() { return grasslandHillsBumpiness; }
    void setGrasslandHillsBumpiness(int inGrasslandHillsBumpiness) { grasslandHillsBumpiness = inGrasslandHillsBumpiness; }
    int& getGrasslandHillsTreeDensity() { return grasslandHillsTreeDensity; }
    void setGrasslandHillsTreeDensity(int inGrasslandHillsTreeDensity) { grasslandHillsTreeDensity = inGrasslandHillsTreeDensity; }
    int& getGrasslandRockyFieldsMaxHeight() { return grasslandRockyFieldsMaxHeight; }
    void setGrasslandRockyFieldsMaxHeight(int inGrasslandRockyFieldsMaxHeight) { grasslandRockyFieldsMaxHeight = inGrasslandRockyFieldsMaxHeight; }
    int& getGrasslandRockyFieldsOccurrenceProbability() { return grasslandRockyFieldsOccurrenceProbability; }
    void setGrasslandRockyFieldsOccurrenceProbability(int inGrasslandRockyFieldsOccurrenceProbability) { grasslandRockyFieldsOccurrenceProbability = inGrasslandRockyFieldsOccurrenceProbability; }
    int& getGrasslandRockyFieldsRockiness() { return grasslandRockyFieldsRockiness; }   
    void setGrasslandRockyFieldsRockiness(int inGrasslandRockyFieldsRockiness) { grasslandRockyFieldsRockiness = inGrasslandRockyFieldsRockiness; }
    int& getGrasslandRockyFieldsTreeDensity() { return grasslandRockyFieldsTreeDensity; }
    void setGrasslandRockyFieldsTreeDensity(int inGrasslandRockyFieldsTreeDensity) { grasslandRockyFieldsTreeDensity = inGrasslandRockyFieldsTreeDensity; }
    int& getGrasslandTerracedFieldsMaxHeight() { return grasslandTerracedFieldsMaxHeight; }
    void setGrasslandTerracedFieldsMaxHeight(int inGrasslandTerracedFieldsMaxHeight) { grasslandTerracedFieldsMaxHeight = inGrasslandTerracedFieldsMaxHeight; }
    int& getGrasslandTerracedFieldsOccurrenceProbability() { return grasslandTerracedFieldsOccurrenceProbability; }
    void setGrasslandTerracedFieldsOccurrenceProbability(int inGrasslandTerracedFieldsOccurrenceProbability) { grasslandTerracedFieldsOccurrenceProbability = inGrasslandTerracedFieldsOccurrenceProbability; }
    int& getGrasslandTerracedFieldsSize() { return grasslandTerracedFieldsSize; }
    void setGrasslandTerracedFieldsSize(int inGrasslandTerracedFieldsSize) { grasslandTerracedFieldsSize = inGrasslandTerracedFieldsSize; }
    int& getGrasslandTerracedFieldsTreeDensity() { return grasslandTerracedFieldsTreeDensity; }
    void setGrasslandTerracedFieldsTreeDensity(int inGrasslandTerracedFieldsTreeDensity) { grasslandTerracedFieldsTreeDensity = inGrasslandTerracedFieldsTreeDensity; }
    int& getGrasslandTerracedFieldsSmoothness() { return grasslandTerracedFieldsSmoothness; }   
    void setGrasslandTerracedFieldsSmoothness(int inGrasslandTerracedFieldsSmoothness) { grasslandTerracedFieldsSmoothness = inGrasslandTerracedFieldsSmoothness; }
    int& getGrasslandTerracedFieldsNumberOfTerraces() { return grasslandTerracedFieldsNumberOfTerraces; }
    void setGrasslandTerracedFieldsNumberOfTerraces(int inGrasslandTerracedFieldsNumberOfTerraces) { grasslandTerracedFieldsNumberOfTerraces = inGrasslandTerracedFieldsNumberOfTerraces; }
    
    // Tundra getters and setters
    bool& getTundraSelected() { return tundraSelected; }
    void setTundraSelected(bool inTundraSelected) { tundraSelected = inTundraSelected; }
    int& getTundraPlainsMaxHeight() { return tundraPlainsMaxHeight; }
    void setTundraPlainsMaxHeight(int inTundraPlainsMaxHeight) { tundraPlainsMaxHeight = inTundraPlainsMaxHeight; }
    int& getTundraPlainsOccurrenceProbability() { return tundraPlainsOccurrenceProbability; }
    void setTundraPlainsOccurrenceProbability(int inTundraPlainsOccurrenceProbability) { tundraPlainsOccurrenceProbability = inTundraPlainsOccurrenceProbability; }
    int& getTundraPlainsEvenness() { return tundraPlainsEvenness; }
    void setTundraPlainsEvenness(int inTundraPlainsEvenness) { tundraPlainsEvenness = inTundraPlainsEvenness; }
    int& getTundraPlainsTreeDensity() { return tundraPlainsTreeDensity; }
    void setTundraPlainsTreeDensity(int inTundraPlainsTreeDensity) { tundraPlainsTreeDensity = inTundraPlainsTreeDensity; } 
    int& getTundraBluntMountainsMaxHeight() { return tundraBluntMountainsMaxHeight; }
    void setTundraBluntMountainsMaxHeight(int inTundraBluntMountainsMaxHeight) { tundraBluntMountainsMaxHeight = inTundraBluntMountainsMaxHeight; }
    int& getTundraBluntMountainsOccurrenceProbability() { return tundraBluntMountainsOccurrenceProbability; }
    void setTundraBluntMountainsOccurrenceProbability(int inTundraBluntMountainsOccurrenceProbability) { tundraBluntMountainsOccurrenceProbability = inTundraBluntMountainsOccurrenceProbability; }
    int& getTundraBluntMountainsRuggedness() { return tundraBluntMountainsRuggedness; }
    void setTundraBluntMountainsRuggedness(int inTundraBluntMountainsRuggedness) { tundraBluntMountainsRuggedness = inTundraBluntMountainsRuggedness; }
    int& getTundraBluntMountainsTreeDensity() { return tundraBluntMountainsTreeDensity; }
    void setTundraBluntMountainsTreeDensity(int inTundraBluntMountainsTreeDensity) { tundraBluntMountainsTreeDensity = inTundraBluntMountainsTreeDensity; }
    int& getTundraPointyMountainsMaxHeight() { return tundraPointyMountainsMaxHeight; }
    void setTundraPointyMountainsMaxHeight(int inTundraPointyMountainsMaxHeight) { tundraPointyMountainsMaxHeight = inTundraPointyMountainsMaxHeight; }
    int& getTundraPointyMountainsOccurrenceProbability() { return tundraPointyMountainsOccurrenceProbability; }
    void setTundraPointyMountainsOccurrenceProbability(int inTundraPointyMountainsOccurrenceProbability) { tundraPointyMountainsOccurrenceProbability = inTundraPointyMountainsOccurrenceProbability; }
    int& getTundraPointyMountainsSteepness() { return tundraPointyMountainsSteepness; }
    void setTundraPointyMountainsSteepness(int inTundraPointyMountainsSteepness) { tundraPointyMountainsSteepness = inTundraPointyMountainsSteepness; }
    int& getTundraPointyMountainsFrequency() { return tundraPointyMountainsFrequency; }
    void setTundraPointyMountainsFrequency(int inTundraPointyMountainsFrequency) { tundraPointyMountainsFrequency = inTundraPointyMountainsFrequency; }
    int& getTundraPointyMountainsTreeDensity() { return tundraPointyMountainsTreeDensity; }
    void setTundraPointyMountainsTreeDensity(int inTundraPointyMountainsTreeDensity) { tundraPointyMountainsTreeDensity = inTundraPointyMountainsTreeDensity; }

    // Savanna getters and setters
    bool& getSavannaSelected() { return savannaSelected; }
    void setSavannaSelected(bool inSavannaSelected) { savannaSelected = inSavannaSelected; }
    int& getSavannaPlainsMaxHeight() { return savannaPlainsMaxHeight; }
    void setSavannaPlainsMaxHeight(int inSavannaPlainsMaxHeight) { savannaPlainsMaxHeight = inSavannaPlainsMaxHeight; }
    int& getSavannaPlainsOccurrenceProbability() { return savannaPlainsOccurrenceProbability; }
    void setSavannaPlainsOccurrenceProbability(int inSavannaPlainsOccurrenceProbability) { savannaPlainsOccurrenceProbability = inSavannaPlainsOccurrenceProbability; }
    int& getSavannaPlainsEvenness() { return savannaPlainsEvenness; }
    void setSavannaPlainsEvenness(int inSavannaPlainsEvenness) { savannaPlainsEvenness = inSavannaPlainsEvenness; }
    int& getSavannaPlainsTreeDensity() { return savannaPlainsTreeDensity; }
    void setSavannaPlainsTreeDensity(int inSavannaPlainsTreeDensity) { savannaPlainsTreeDensity = inSavannaPlainsTreeDensity; }
    int& getSavannaMountainsMaxHeight() { return savannaMountainsMaxHeight; }
    void setSavannaMountainsMaxHeight(int inSavannaMountainsMaxHeight) { savannaMountainsMaxHeight = inSavannaMountainsMaxHeight; }
    int& getSavannaMountainsOccurrenceProbability() { return savannaMountainsOccurrenceProbability; }
    void setSavannaMountainsOccurrenceProbability(int inSavannaMountainsOccurrenceProbability) { savannaMountainsOccurrenceProbability = inSavannaMountainsOccurrenceProbability; }
    int& getSavannaMountainsRuggedness() { return savannaMountainsRuggedness; }
    void setSavannaMountainsRuggedness(int inSavannaMountainsRuggedness) { savannaMountainsRuggedness = inSavannaMountainsRuggedness; }
    int& getSavannaMountainsTreeDensity() { return savannaMountainsTreeDensity; }
    void setSavannaMountainsTreeDensity(int inSavannaMountainsTreeDensity) { savannaMountainsTreeDensity = inSavannaMountainsTreeDensity; }

    // Woodland getters and setters
    bool& getWoodlandSelected() { return woodlandSelected; }
    void setWoodlandSelected(bool inWoodlandSelected) { woodlandSelected = inWoodlandSelected; }
    int& getWoodlandHillsMaxHeight() { return woodlandHillsMaxHeight; }
    void setWoodlandHillsMaxHeight(int inWoodlandHillsMaxHeight) { woodlandHillsMaxHeight = inWoodlandHillsMaxHeight; }
    int& getWoodlandHillsOccurrenceProbability() { return woodlandHillsOccurrenceProbability; }
    void setWoodlandHillsOccurrenceProbability(int inWoodlandHillsOccurrenceProbability) { woodlandHillsOccurrenceProbability = inWoodlandHillsOccurrenceProbability; }
    int& getWoodlandHillsBumpiness() { return woodlandHillsBumpiness; }
    void setWoodlandHillsBumpiness(int inWoodlandHillsBumpiness) { woodlandHillsBumpiness = inWoodlandHillsBumpiness; }
    int& getWoodlandHillsTreeDensity() { return woodlandHillsTreeDensity; }
    void setWoodlandHillsTreeDensity(int inWoodlandHillsTreeDensity) { woodlandHillsTreeDensity = inWoodlandHillsTreeDensity; }

    // Tropical Rainforest getters and setters
    bool& getTropicalRainforestSelected() { return tropicalRainforestSelected; }
    void setTropicalRainforestSelected(bool inTropicalRainforestSelected) { tropicalRainforestSelected = inTropicalRainforestSelected; }
    int& getTropicalRainforestPlainsMaxHeight() { return tropicalRainforestPlainsMaxHeight; }
    void setTropicalRainforestPlainsMaxHeight(int inTropicalRainforestPlainsMaxHeight) { tropicalRainforestPlainsMaxHeight = inTropicalRainforestPlainsMaxHeight; }
    int& getTropicalRainforestPlainsOccurrenceProbability() { return tropicalRainforestPlainsOccurrenceProbability; }
    void setTropicalRainforestPlainsOccurrenceProbability(int inTropicalRainforestPlainsOccurrenceProbability) { tropicalRainforestPlainsOccurrenceProbability = inTropicalRainforestPlainsOccurrenceProbability; }
    int& getTropicalRainforestPlainsEvenness() { return tropicalRainforestPlainsEvenness; }
    void setTropicalRainforestPlainsEvenness(int inTropicalRainforestPlainsEvenness) { tropicalRainforestPlainsEvenness = inTropicalRainforestPlainsEvenness; }
    int& getTropicalRainforestPlainsTreeDensity() { return tropicalRainforestPlainsTreeDensity; }
    void setTropicalRainforestPlainsTreeDensity(int inTropicalRainforestPlainsTreeDensity) { tropicalRainforestPlainsTreeDensity = inTropicalRainforestPlainsTreeDensity; }
    int& getTropicalRainforestMountainsMaxHeight() { return tropicalRainforestMountainsMaxHeight; }
    void setTropicalRainforestMountainsMaxHeight(int inTropicalRainforestMountainsMaxHeight) { tropicalRainforestMountainsMaxHeight = inTropicalRainforestMountainsMaxHeight; }
    int& getTropicalRainforestMountainsOccurrenceProbability() { return tropicalRainforestMountainsOccurrenceProbability; }
    void setTropicalRainforestMountainsOccurrenceProbability(int inTropicalRainforestMountainsOccurrenceProbability) { tropicalRainforestMountainsOccurrenceProbability = inTropicalRainforestMountainsOccurrenceProbability; }
    int& getTropicalRainforestMountainsRuggedness() { return tropicalRainforestMountainsRuggedness; }
    void setTropicalRainforestMountainsRuggedness(int inTropicalRainforestMountainsRuggedness) { tropicalRainforestMountainsRuggedness = inTropicalRainforestMountainsRuggedness; }
    int& getTropicalRainforestMountainsTreeDensity() { return tropicalRainforestMountainsTreeDensity; }
    void setTropicalRainforestMountainsTreeDensity(int inTropicalRainforestMountainsTreeDensity) { tropicalRainforestMountainsTreeDensity = inTropicalRainforestMountainsTreeDensity; }
    int& getTropicalRainforestHillsMaxHeight() { return tropicalRainforestHillsMaxHeight; }
    void setTropicalRainforestHillsMaxHeight(int inTropicalRainforestHillsMaxHeight) { tropicalRainforestHillsMaxHeight = inTropicalRainforestHillsMaxHeight; }
    int& getTropicalRainforestHillsOccurrenceProbability() { return tropicalRainforestHillsOccurrenceProbability; }
    void setTropicalRainforestHillsOccurrenceProbability(int inTropicalRainforestHillsOccurrenceProbability) { tropicalRainforestHillsOccurrenceProbability = inTropicalRainforestHillsOccurrenceProbability; }
    int& getTropicalRainforestHillsBumpiness() { return tropicalRainforestHillsBumpiness; }
    void setTropicalRainforestHillsBumpiness(int inTropicalRainforestHillsBumpiness) { tropicalRainforestHillsBumpiness = inTropicalRainforestHillsBumpiness; }
    int& getTropicalRainforestHillsTreeDensity() { return tropicalRainforestHillsTreeDensity; }
    void setTropicalRainforestHillsTreeDensity(int inTropicalRainforestHillsTreeDensity) { tropicalRainforestHillsTreeDensity = inTropicalRainforestHillsTreeDensity; }
    int& getTropicalRainforestVolcanoesMaxHeight() { return tropicalRainforestVolcanoesMaxHeight; }
    void setTropicalRainforestVolcanoesMaxHeight(int inTropicalRainforestVolcanoesMaxHeight) { tropicalRainforestVolcanoesMaxHeight = inTropicalRainforestVolcanoesMaxHeight; }
    int& getTropicalRainforestVolcanoesOccurrenceProbability() { return tropicalRainforestVolcanoesOccurrenceProbability; }
    void setTropicalRainforestVolcanoesOccurrenceProbability(int inTropicalRainforestVolcanoesOccurrenceProbability) { tropicalRainforestVolcanoesOccurrenceProbability = inTropicalRainforestVolcanoesOccurrenceProbability; }
    int& getTropicalRainforestVolcanoesSize() { return tropicalRainforestVolcanoesSize; }
    void setTropicalRainforestVolcanoesSize(int inTropicalRainforestVolcanoesSize) { tropicalRainforestVolcanoesSize = inTropicalRainforestVolcanoesSize; }
    int& getTropicalRainforestVolcanoesTreeDensity() { return tropicalRainforestVolcanoesTreeDensity; }
    void setTropicalRainforestVolcanoesTreeDensity(int inTropicalRainforestVolcanoesTreeDensity) { tropicalRainforestVolcanoesTreeDensity = inTropicalRainforestVolcanoesTreeDensity; }
    int& getTropicalRainforestVolcanoesThickness() { return tropicalRainforestVolcanoesThickness; }
    void setTropicalRainforestVolcanoesThickness(int inTropicalRainforestVolcanoesThickness) { tropicalRainforestVolcanoesThickness = inTropicalRainforestVolcanoesThickness; }
    int& getTropicalRainforestVolcanoesDensity() { return tropicalRainforestVolcanoesDensity; }
    void setTropicalRainforestVolcanoesDensity(int inTropicalRainforestVolcanoesDensity) { tropicalRainforestVolcanoesDensity = inTropicalRainforestVolcanoesDensity; }
    
    // Temperate Rainforest getters and setters
    bool& getTemperateRainforestSelected() { return temperateRainforestSelected; }
    void setTemperateRainforestSelected(bool inTemperateRainforestSelected) { temperateRainforestSelected = inTemperateRainforestSelected; }
    int& getTemperateRainforestHillsMaxHeight() { return temperateRainforestHillsMaxHeight; }
    void setTemperateRainforestHillsMaxHeight(int inTemperateRainforestHillsMaxHeight) { temperateRainforestHillsMaxHeight = inTemperateRainforestHillsMaxHeight; }
    int& getTemperateRainforestHillsOccurrenceProbability() { return temperateRainforestHillsOccurrenceProbability; }
    void setTemperateRainforestHillsOccurrenceProbability(int inTemperateRainforestHillsOccurrenceProbability) { temperateRainforestHillsOccurrenceProbability = inTemperateRainforestHillsOccurrenceProbability; }
    int& getTemperateRainforestHillsBumpiness() { return temperateRainforestHillsBumpiness; }
    void setTemperateRainforestHillsBumpiness(int inTemperateRainforestHillsBumpiness) { temperateRainforestHillsBumpiness = inTemperateRainforestHillsBumpiness; }
    int& getTemperateRainforestHillsTreeDensity() { return temperateRainforestHillsTreeDensity; }
    void setTemperateRainforestHillsTreeDensity(int inTemperateRainforestHillsTreeDensity) { temperateRainforestHillsTreeDensity = inTemperateRainforestHillsTreeDensity; }
    int& getTemperateRainforestMountainsMaxHeight() { return temperateRainforestMountainsMaxHeight; }
    void setTemperateRainforestMountainsMaxHeight(int inTemperateRainforestMountainsMaxHeight) { temperateRainforestMountainsMaxHeight = inTemperateRainforestMountainsMaxHeight; }
    int& getTemperateRainforestMountainsOccurrenceProbability() { return temperateRainforestMountainsOccurrenceProbability; }
    void setTemperateRainforestMountainsOccurrenceProbability(int inTemperateRainforestMountainsOccurrenceProbability) { temperateRainforestMountainsOccurrenceProbability = inTemperateRainforestMountainsOccurrenceProbability; }
    int& getTemperateRainforestMountainsRuggedness() { return temperateRainforestMountainsRuggedness; }
    void setTemperateRainforestMountainsRuggedness(int inTemperateRainforestMountainsRuggedness) { temperateRainforestMountainsRuggedness = inTemperateRainforestMountainsRuggedness; }
    int& getTemperateRainforestMountainsTreeDensity() { return temperateRainforestMountainsTreeDensity; }
    void setTemperateRainforestMountainsTreeDensity(int inTemperateRainforestMountainsTreeDensity) { temperateRainforestMountainsTreeDensity = inTemperateRainforestMountainsTreeDensity; }
    int& getTemperateRainforestSwampMaxHeight() { return temperateRainforestSwampMaxHeight; }
    void setTemperateRainforestSwampMaxHeight(int inTemperateRainforestSwampMaxHeight) { temperateRainforestSwampMaxHeight = inTemperateRainforestSwampMaxHeight; }
    int& getTemperateRainforestSwampOccurrenceProbability() { return temperateRainforestSwampOccurrenceProbability; }
    void setTemperateRainforestSwampOccurrenceProbability(int inTemperateRainforestSwampOccurrenceProbability) { temperateRainforestSwampOccurrenceProbability = inTemperateRainforestSwampOccurrenceProbability; }
    int& getTemperateRainforestSwampWetness() { return temperateRainforestSwampWetness; }
    void setTemperateRainforestSwampWetness(int inTemperateRainforestSwampWetness) { temperateRainforestSwampWetness = inTemperateRainforestSwampWetness; }
    int& getTemperateRainforestSwampTreeDensity() { return temperateRainforestSwampTreeDensity; }
    void setTemperateRainforestSwampTreeDensity(int inTemperateRainforestSwampTreeDensity) { temperateRainforestSwampTreeDensity = inTemperateRainforestSwampTreeDensity; }
    
    // Temperate Seasonal Forest getters and setters
    bool& getTemperateSeasonalForestSelected() { return temperateSeasonalForestSelected; }
    void setTemperateSeasonalForestSelected(bool inTemperateSeasonalForestSelected) { temperateSeasonalForestSelected = inTemperateSeasonalForestSelected; }
    int& getTemperateSeasonalForestHillsMaxHeight() { return temperateSeasonalForestHillsMaxHeight; }
    void setTemperateSeasonalForestHillsMaxHeight(int inTemperateSeasonalForestHillsMaxHeight) { temperateSeasonalForestHillsMaxHeight = inTemperateSeasonalForestHillsMaxHeight; }
    int& getTemperateSeasonalForestHillsOccurrenceProbability() { return temperateSeasonalForestHillsOccurrenceProbability; }
    void setTemperateSeasonalForestHillsOccurrenceProbability(int inTemperateSeasonalForestHillsOccurrenceProbability) { temperateSeasonalForestHillsOccurrenceProbability = inTemperateSeasonalForestHillsOccurrenceProbability; }
    int& getTemperateSeasonalForestHillsBumpiness() { return temperateSeasonalForestHillsBumpiness; }
    void setTemperateSeasonalForestHillsBumpiness(int inTemperateSeasonalForestHillsBumpiness) { temperateSeasonalForestHillsBumpiness = inTemperateSeasonalForestHillsBumpiness; }
    int& getTemperateSeasonalForestHillsTreeDensity() { return temperateSeasonalForestHillsTreeDensity; }
    void setTemperateSeasonalForestHillsTreeDensity(int inTemperateSeasonalForestHillsTreeDensity) { temperateSeasonalForestHillsTreeDensity = inTemperateSeasonalForestHillsTreeDensity; }
    int& getTemperateSeasonalForestHillsAutumnalOccurrence() { return temperateSeasonalForestHillsAutumnalOccurrence; }
    void setTemperateSeasonalForestHillsAutumnalOccurrence(int inTemperateSeasonalForestHillsAutumnalOccurrence) { temperateSeasonalForestHillsAutumnalOccurrence = inTemperateSeasonalForestHillsAutumnalOccurrence; }
    int& getTemperateSeasonalForestMountainsMaxHeight() { return temperateSeasonalForestMountainsMaxHeight; }
    void setTemperateSeasonalForestMountainsMaxHeight(int inTemperateSeasonalForestMountainsMaxHeight) { temperateSeasonalForestMountainsMaxHeight = inTemperateSeasonalForestMountainsMaxHeight; }
    int& getTemperateSeasonalForestMountainsOccurrenceProbability() { return temperateSeasonalForestMountainsOccurrenceProbability; }
    void setTemperateSeasonalForestMountainsOccurrenceProbability(int inTemperateSeasonalForestMountainsOccurrenceProbability) { temperateSeasonalForestMountainsOccurrenceProbability = inTemperateSeasonalForestMountainsOccurrenceProbability; }
    int& getTemperateSeasonalForestMountainsRuggedness() { return temperateSeasonalForestMountainsRuggedness; }
    void setTemperateSeasonalForestMountainsRuggedness(int inTemperateSeasonalForestMountainsRuggedness) { temperateSeasonalForestMountainsRuggedness = inTemperateSeasonalForestMountainsRuggedness; }
    int& getTemperateSeasonalForestMountainsTreeDensity() { return temperateSeasonalForestMountainsTreeDensity; }
    void setTemperateSeasonalForestMountainsTreeDensity(int inTemperateSeasonalForestMountainsTreeDensity) { temperateSeasonalForestMountainsTreeDensity = inTemperateSeasonalForestMountainsTreeDensity; }
    int& getTemperateSeasonalForestMountainsAutumnalOccurrence() { return temperateSeasonalForestMountainsAutumnalOccurrence; }
    void setTemperateSeasonalForestMountainsAutumnalOccurrence(int inTemperateSeasonalForestMountainsAutumnalOccurrence) { temperateSeasonalForestMountainsAutumnalOccurrence = inTemperateSeasonalForestMountainsAutumnalOccurrence; }

    // Subtropical Desert getters and setters
    bool& getSubtropicalDesertSelected() { return subtropicalDesertSelected; }
    void setSubtropicalDesertSelected(bool inSubtropicalDesertSelected) { subtropicalDesertSelected = inSubtropicalDesertSelected; }
    int& getSubtropicalDesertDunesMaxHeight() { return subtropicalDesertDunesMaxHeight; }
    void setSubtropicalDesertDunesMaxHeight(int inSubtropicalDesertDunesMaxHeight) { subtropicalDesertDunesMaxHeight = inSubtropicalDesertDunesMaxHeight; }
    int& getSubtropicalDesertDunesOccurrenceProbability() { return subtropicalDesertDunesOccurrenceProbability; }
    void setSubtropicalDesertDunesOccurrenceProbability(int inSubtropicalDesertDunesOccurrenceProbability) { subtropicalDesertDunesOccurrenceProbability = inSubtropicalDesertDunesOccurrenceProbability; }
    int& getSubtropicalDesertDunesSize() { return subtropicalDesertDunesSize; }
    void setSubtropicalDesertDunesSize(int inSubtropicalDesertDunesSize) { subtropicalDesertDunesSize = inSubtropicalDesertDunesSize; }
    int& getSubtropicalDesertDunesTreeDensity() { return subtropicalDesertDunesTreeDensity; }
    void setSubtropicalDesertDunesTreeDensity(int inSubtropicalDesertDunesTreeDensity) { subtropicalDesertDunesTreeDensity = inSubtropicalDesertDunesTreeDensity; }
    int& getSubtropicalDesertDunesDuneFrequency() { return subtropicalDesertDunesDuneFrequency; }
    void setSubtropicalDesertDunesDuneFrequency(int inSubtropicalDesertDunesDuneFrequency) { subtropicalDesertDunesDuneFrequency = inSubtropicalDesertDunesDuneFrequency; }
    int& getSubtropicalDesertDunesDuneWaviness() { return subtropicalDesertDunesDuneWaviness; }
    void setSubtropicalDesertDunesDuneWaviness(int inSubtropicalDesertDunesDuneWaviness) { subtropicalDesertDunesDuneWaviness = inSubtropicalDesertDunesDuneWaviness; }
    int& getSubtropicalDesertDunesBumpiness() { return subtropicalDesertDunesBumpiness; }
    void setSubtropicalDesertDunesBumpiness(int inSubtropicalDesertDunesBumpiness) { subtropicalDesertDunesBumpiness = inSubtropicalDesertDunesBumpiness; }
    int& getSubtropicalDesertMesasMaxHeight() { return subtropicalDesertMesasMaxHeight; }
    void setSubtropicalDesertMesasMaxHeight(int inSubtropicalDesertMesasMaxHeight) { subtropicalDesertMesasMaxHeight = inSubtropicalDesertMesasMaxHeight; }
    int& getSubtropicalDesertMesasOccurrenceProbability() { return subtropicalDesertMesasOccurrenceProbability; }
    void setSubtropicalDesertMesasOccurrenceProbability(int inSubtropicalDesertMesasOccurrenceProbability) { subtropicalDesertMesasOccurrenceProbability = inSubtropicalDesertMesasOccurrenceProbability; }
    int& getSubtropicalDesertMesasSize() { return subtropicalDesertMesasSize; }
    void setSubtropicalDesertMesasSize(int inSubtropicalDesertMesasSize) { subtropicalDesertMesasSize = inSubtropicalDesertMesasSize; }
    int& getSubtropicalDesertMesasTreeDensity() { return subtropicalDesertMesasTreeDensity; }
    void setSubtropicalDesertMesasTreeDensity(int inSubtropicalDesertMesasTreeDensity) { subtropicalDesertMesasTreeDensity = inSubtropicalDesertMesasTreeDensity; }
    int& getSubtropicalDesertMesasNumberOfTerraces() { return subtropicalDesertMesasNumberOfTerraces; }
    void setSubtropicalDesertMesasNumberOfTerraces(int inSubtropicalDesertMesasNumberOfTerraces) { subtropicalDesertMesasNumberOfTerraces = inSubtropicalDesertMesasNumberOfTerraces; }
    int& getSubtropicalDesertMesasSteepness() { return subtropicalDesertMesasSteepness; }
    void setSubtropicalDesertMesasSteepness(int inSubtropicalDesertMesasSteepness) { subtropicalDesertMesasSteepness = inSubtropicalDesertMesasSteepness; }
    int& getSubtropicalDesertRavinesMaxHeight() { return subtropicalDesertRavinesMaxHeight; }
    void setSubtropicalDesertRavinesMaxHeight(int inSubtropicalDesertRavinesMaxHeight) { subtropicalDesertRavinesMaxHeight = inSubtropicalDesertRavinesMaxHeight; }
    int& getSubtropicalDesertRavinesOccurrenceProbability() { return subtropicalDesertRavinesOccurrenceProbability; }
    void setSubtropicalDesertRavinesOccurrenceProbability(int inSubtropicalDesertRavinesOccurrenceProbability) { subtropicalDesertRavinesOccurrenceProbability = inSubtropicalDesertRavinesOccurrenceProbability; }
    int& getSubtropicalDesertRavinesDensity() { return subtropicalDesertRavinesDensity; }
    void setSubtropicalDesertRavinesDensity(int inSubtropicalDesertRavinesDensity) { subtropicalDesertRavinesDensity = inSubtropicalDesertRavinesDensity; }
    int& getSubtropicalDesertRavinesTreeDensity() { return subtropicalDesertRavinesTreeDensity; }
    void setSubtropicalDesertRavinesTreeDensity(int inSubtropicalDesertRavinesTreeDensity) { subtropicalDesertRavinesTreeDensity = inSubtropicalDesertRavinesTreeDensity; }
    int& getSubtropicalDesertRavinesRavineWidth() { return subtropicalDesertRavinesRavineWidth; }
    void setSubtropicalDesertRavinesRavineWidth(int inSubtropicalDesertRavinesRavineWidth) { subtropicalDesertRavinesRavineWidth = inSubtropicalDesertRavinesRavineWidth; }
    int& getSubtropicalDesertRavinesSmoothness() { return subtropicalDesertRavinesSmoothness; }
    void setSubtropicalDesertRavinesSmoothness(int inSubtropicalDesertRavinesSmoothness) { subtropicalDesertRavinesSmoothness = inSubtropicalDesertRavinesSmoothness; }
    int& getSubtropicalDesertRavinesSteepness() { return subtropicalDesertRavinesSteepness; }
    void setSubtropicalDesertRavinesSteepness(int inSubtropicalDesertRavinesSteepness) { subtropicalDesertRavinesSteepness = inSubtropicalDesertRavinesSteepness; }
    int& getSubtropicalDesertOasisMaxHeight() { return subtropicalDesertOasisMaxHeight; }
    void setSubtropicalDesertOasisMaxHeight(int inSubtropicalDesertOasisMaxHeight) { subtropicalDesertOasisMaxHeight = inSubtropicalDesertOasisMaxHeight; }
    int& getSubtropicalDesertOasisOccurrenceProbability() { return subtropicalDesertOasisOccurrenceProbability; }
    void setSubtropicalDesertOasisOccurrenceProbability(int inSubtropicalDesertOasisOccurrenceProbability) { subtropicalDesertOasisOccurrenceProbability = inSubtropicalDesertOasisOccurrenceProbability; }
    int& getSubtropicalDesertOasisSize() { return subtropicalDesertOasisSize; }
    void setSubtropicalDesertOasisSize(int inSubtropicalDesertOasisSize) { subtropicalDesertOasisSize = inSubtropicalDesertOasisSize; }
    int& getSubtropicalDesertOasisFlatness() { return subtropicalDesertOasisFlatness; }
    void setSubtropicalDesertOasisFlatness(int inSubtropicalDesertOasisFlatness) { subtropicalDesertOasisFlatness = inSubtropicalDesertOasisFlatness; }
    int& getSubtropicalDesertOasisTreeDensity() { return subtropicalDesertOasisTreeDensity; }
    void setSubtropicalDesertOasisTreeDensity(int inSubtropicalDesertOasisTreeDensity) { subtropicalDesertOasisTreeDensity = inSubtropicalDesertOasisTreeDensity; }
    int& getSubtropicalDesertOasisDuneFrequency() { return subtropicalDesertOasisDuneFrequency; }
    void setSubtropicalDesertOasisDuneFrequency(int inSubtropicalDesertOasisDuneFrequency) { subtropicalDesertOasisDuneFrequency = inSubtropicalDesertOasisDuneFrequency; }
    int& getSubtropicalDesertCrackedMaxHeight() { return subtropicalDesertCrackedMaxHeight; }
    void setSubtropicalDesertCrackedMaxHeight(int inSubtropicalDesertCrackedMaxHeight) { subtropicalDesertCrackedMaxHeight = inSubtropicalDesertCrackedMaxHeight; }
    int& getSubtropicalDesertCrackedOccurrenceProbability() { return subtropicalDesertCrackedOccurrenceProbability; }
    void setSubtropicalDesertCrackedOccurrenceProbability(int inSubtropicalDesertCrackedOccurrenceProbability) { subtropicalDesertCrackedOccurrenceProbability = inSubtropicalDesertCrackedOccurrenceProbability; }
    int& getSubtropicalDesertCrackedSize() { return subtropicalDesertCrackedSize; }
    void setSubtropicalDesertCrackedSize(int inSubtropicalDesertCrackedSize) { subtropicalDesertCrackedSize = inSubtropicalDesertCrackedSize; }
    int& getSubtropicalDesertCrackedFlatness() { return subtropicalDesertCrackedFlatness; }
    void setSubtropicalDesertCrackedFlatness(int inSubtropicalDesertCrackedFlatness) { subtropicalDesertCrackedFlatness = inSubtropicalDesertCrackedFlatness; }
    int& getSubtropicalDesertCrackedTreeDensity() { return subtropicalDesertCrackedTreeDensity; }
    void setSubtropicalDesertCrackedTreeDensity(int inSubtropicalDesertCrackedTreeDensity) { subtropicalDesertCrackedTreeDensity = inSubtropicalDesertCrackedTreeDensity; }

    // Ocean getters and setters
    int& getOceanFlatSeabedMaxHeight() { return oceanFlatSeabedMaxHeight; }
    void setOceanFlatSeabedMaxHeight(int inOceanFlatSeabedMaxHeight) { oceanFlatSeabedMaxHeight = inOceanFlatSeabedMaxHeight; }
    int& getOceanFlatSeabedEvenness() { return oceanFlatSeabedEvenness; }
    void setOceanFlatSeabedEvenness(int inOceanFlatSeabedEvenness) { oceanFlatSeabedEvenness = inOceanFlatSeabedEvenness; }
    int& getOceanFlatSeabedOccurrenceProbability() { return oceanFlatSeabedOccurrenceProbability; }
    void setOceanFlatSeabedOccurrenceProbability(int inOceanFlatSeabedOccurrenceProbability) { oceanFlatSeabedOccurrenceProbability = inOceanFlatSeabedOccurrenceProbability; }
    int& getOceanVolcanicIslandsMaxHeight() { return oceanVolcanicIslandsMaxHeight; }
    void setOceanVolcanicIslandsMaxHeight(int inOceanVolcanicIslandsMaxHeight) { oceanVolcanicIslandsMaxHeight = inOceanVolcanicIslandsMaxHeight; }
    int& getOceanVolcanicIslandsOccurrenceProbability() { return oceanVolcanicIslandsOccurrenceProbability; }
    void setOceanVolcanicIslandsOccurrenceProbability(int inOceanVolcanicIslandsOccurrenceProbability) { oceanVolcanicIslandsOccurrenceProbability = inOceanVolcanicIslandsOccurrenceProbability; }
    int& getOceanVolcanicIslandsSize() { return oceanVolcanicIslandsSize; }
    void setOceanVolcanicIslandsSize(int inOceanVolcanicIslandsSize) { oceanVolcanicIslandsSize = inOceanVolcanicIslandsSize; }
    int& getOceanVolcanicIslandsThickness() { return oceanVolcanicIslandsThickness; }
    void setOceanVolcanicIslandsThickness(int inOceanVolcanicIslandsThickness) { oceanVolcanicIslandsThickness = inOceanVolcanicIslandsThickness; }
    int& getOceanVolcanicIslandsDensity() { return oceanVolcanicIslandsDensity; }
    void setOceanVolcanicIslandsDensity(int inOceanVolcanicIslandsDensity) { oceanVolcanicIslandsDensity = inOceanVolcanicIslandsDensity; }
    int& getOceanWaterStacksMaxHeight() { return oceanWaterStacksMaxHeight; }
    void setOceanWaterStacksMaxHeight(int inOceanWaterStacksMaxHeight) { oceanWaterStacksMaxHeight = inOceanWaterStacksMaxHeight; }
    int& getOceanWaterStacksOccurrenceProbability() { return oceanWaterStacksOccurrenceProbability; }
    void setOceanWaterStacksOccurrenceProbability(int inOceanWaterStacksOccurrenceProbability) { oceanWaterStacksOccurrenceProbability = inOceanWaterStacksOccurrenceProbability; }
    int& getOceanWaterStacksSize() { return oceanWaterStacksSize; }
    void setOceanWaterStacksSize(int inOceanWaterStacksSize) { oceanWaterStacksSize = inOceanWaterStacksSize; }
    int& getOceanTrenchesDensity() { return oceanTrenchesDensity; }
    void setOceanTrenchesDensity(int inOceanTrenchesDensity) { oceanTrenchesDensity = inOceanTrenchesDensity; }
    int& getOceanTrenchesOccurrenceProbability() { return oceanTrenchesOccurrenceProbability; }
    void setOceanTrenchesOccurrenceProbability(int inOceanTrenchesOccurrenceProbability) { oceanTrenchesOccurrenceProbability = inOceanTrenchesOccurrenceProbability; }
    int& getOceanTrenchesTrenchWidth() { return oceanTrenchesTrenchWidth; }
    void setOceanTrenchesTrenchWidth(int inOceanTrenchesTrenchWidth) { oceanTrenchesTrenchWidth = inOceanTrenchesTrenchWidth; }
    int& getOceanTrenchesSmoothness() { return oceanTrenchesSmoothness; }
    void setOceanTrenchesSmoothness(int inOceanTrenchesSmoothness) { oceanTrenchesSmoothness = inOceanTrenchesSmoothness; }

    // Texture getters and setters
    string getBorealTextureLow() { return borealTextureLow; }
    void setBorealTextureLow(string inBorealTextureLow) { borealTextureLow = inBorealTextureLow; }
    string getBorealTextureMidFlat() { return borealTextureMidFlat; }
    void setBorealTextureMidFlat(string inBorealTextureMidFlat) { borealTextureMidFlat = inBorealTextureMidFlat; }
    string getBorealTextureMidSteep() { return borealTextureMidSteep; }
    void setBorealTextureMidSteep(string inBorealTextureMidSteep) { borealTextureMidSteep = inBorealTextureMidSteep; }
    string getBorealTextureHigh() { return borealTextureHigh; }
    void setBorealTextureHigh(string inBorealTextureHigh) { borealTextureHigh = inBorealTextureHigh; }

    string getGrassyTextureLow() { return grassyTextureLow; }
    void setGrassyTextureLow(string inGrassyTextureLow) { grassyTextureLow = inGrassyTextureLow; }
    string getGrassyTextureMidFlat() { return grassyTextureMidFlat; }
    void setGrassyTextureMidFlat(string inGrassyTextureMidFlat) { grassyTextureMidFlat = inGrassyTextureMidFlat; }
    string getGrassyTextureMidSteep() { return grassyTextureMidSteep; }
    void setGrassyTextureMidSteep(string inGrassyTextureMidSteep) { grassyTextureMidSteep = inGrassyTextureMidSteep; }
    string getGrassyTextureHigh() { return grassyTextureHigh; }
    void setGrassyTextureHigh(string inGrassyTextureHigh) { grassyTextureHigh = inGrassyTextureHigh; }

    string getGrassyStoneTextureLow() { return grassyStoneTextureLow; }
    void setGrassyStoneTextureLow(string inGrassyStoneTextureLow) { grassyStoneTextureLow = inGrassyStoneTextureLow; }
    string getGrassyStoneTextureMidFlat() { return grassyStoneTextureMidFlat; }
    void setGrassyStoneTextureMidFlat(string inGrassyStoneTextureMidFlat) { grassyStoneTextureMidFlat = inGrassyStoneTextureMidFlat; }
    string getGrassyStoneTextureMidSteep() { return grassyStoneTextureMidSteep; }
    void setGrassyStoneTextureMidSteep(string inGrassyStoneTextureMidSteep) { grassyStoneTextureMidSteep = inGrassyStoneTextureMidSteep; }
    string getGrassyStoneTextureHigh() { return grassyStoneTextureHigh; }
    void setGrassyStoneTextureHigh(string inGrassyStoneTextureHigh) { grassyStoneTextureHigh = inGrassyStoneTextureHigh; }

    string getSnowyTextureLow() { return snowyTextureLow; }
    void setSnowyTextureLow(string inSnowyTextureLow) { snowyTextureLow = inSnowyTextureLow; }
    string getSnowyTextureMidFlat() { return snowyTextureMidFlat; }
    void setSnowyTextureMidFlat(string inSnowyTextureMidFlat) { snowyTextureMidFlat = inSnowyTextureMidFlat; }
    string getSnowyTextureMidSteep() { return snowyTextureMidSteep; }
    void setSnowyTextureMidSteep(string inSnowyTextureMidSteep) { snowyTextureMidSteep = inSnowyTextureMidSteep; }
    string getSnowyTextureHigh() { return snowyTextureHigh; }
    void setSnowyTextureHigh(string inSnowyTextureHigh) { snowyTextureHigh = inSnowyTextureHigh; }

    string getIcyTextureLow() { return icyTextureLow; }
    void setIcyTextureLow(string inIcyTextureLow) { icyTextureLow = inIcyTextureLow; }
    string getIcyTextureMidFlat() { return icyTextureMidFlat; }
    void setIcyTextureMidFlat(string inIcyTextureMidFlat) { icyTextureMidFlat = inIcyTextureMidFlat; }
    string getIcyTextureMidSteep() { return icyTextureMidSteep; }
    void setIcyTextureMidSteep(string inIcyTextureMidSteep) { icyTextureMidSteep = inIcyTextureMidSteep; }
    string getIcyTextureHigh() { return icyTextureHigh; }
    void setIcyTextureHigh(string inIcyTextureHigh) { icyTextureHigh = inIcyTextureHigh; }

    string getSavannaTextureLow() { return savannaTextureLow; }
    void setSavannaTextureLow(string inSavannaTextureLow) { savannaTextureLow = inSavannaTextureLow; }
    string getSavannaTextureMidFlat() { return savannaTextureMidFlat; }
    void setSavannaTextureMidFlat(string inSavannaTextureMidFlat) { savannaTextureMidFlat = inSavannaTextureMidFlat; }
    string getSavannaTextureMidSteep() { return savannaTextureMidSteep; }
    void setSavannaTextureMidSteep(string inSavannaTextureMidSteep) { savannaTextureMidSteep = inSavannaTextureMidSteep; }
    string getSavannaTextureHigh() { return savannaTextureHigh; }
    void setSavannaTextureHigh(string inSavannaTextureHigh) { savannaTextureHigh = inSavannaTextureHigh; }

    string getWoodlandTextureLow() { return woodlandTextureLow; }
    void setWoodlandTextureLow(string inWoodlandTextureLow) { woodlandTextureLow = inWoodlandTextureLow; }
    string getWoodlandTextureMidFlat() { return woodlandTextureMidFlat; }
    void setWoodlandTextureMidFlat(string inWoodlandTextureMidFlat) { woodlandTextureMidFlat = inWoodlandTextureMidFlat; }
    string getWoodlandTextureMidSteep() { return woodlandTextureMidSteep; }
    void setWoodlandTextureMidSteep(string inWoodlandTextureMidSteep) { woodlandTextureMidSteep = inWoodlandTextureMidSteep; }
    string getWoodlandTextureHigh() { return woodlandTextureHigh; }
    void setWoodlandTextureHigh(string inWoodlandTextureHigh) { woodlandTextureHigh = inWoodlandTextureHigh; }

    string getJungleTextureLow() { return jungleTextureLow; }
    void setJungleTextureLow(string inJungleTextureLow) { jungleTextureLow = inJungleTextureLow; }
    string getJungleTextureMidFlat() { return jungleTextureMidFlat; }
    void setJungleTextureMidFlat(string inJungleTextureMidFlat) { jungleTextureMidFlat = inJungleTextureMidFlat; }
    string getJungleTextureMidSteep() { return jungleTextureMidSteep; }
    void setJungleTextureMidSteep(string inJungleTextureMidSteep) { jungleTextureMidSteep = inJungleTextureMidSteep; }
    string getJungleTextureHigh() { return jungleTextureHigh; }
    void setJungleTextureHigh(string inJungleTextureHigh) { jungleTextureHigh = inJungleTextureHigh; }

    string getJungleMountainsTextureLow() { return jungleMountainsTextureLow; }
    void setJungleMountainsTextureLow(string inJungleMountainsTextureLow) { jungleMountainsTextureLow = inJungleMountainsTextureLow; }
    string getJungleMountainsTextureMidFlat() { return jungleMountainsTextureMidFlat; }
    void setJungleMountainsTextureMidFlat(string inJungleMountainsTextureMidFlat) { jungleMountainsTextureMidFlat = inJungleMountainsTextureMidFlat; }
    string getJungleMountainsTextureMidSteep() { return jungleMountainsTextureMidSteep; }
    void setJungleMountainsTextureMidSteep(string inJungleMountainsTextureMidSteep) { jungleMountainsTextureMidSteep = inJungleMountainsTextureMidSteep; }
    string getJungleMountainsTextureHigh() { return jungleMountainsTextureHigh; }
    void setJungleMountainsTextureHigh(string inJungleMountainsTextureHigh) { jungleMountainsTextureHigh = inJungleMountainsTextureHigh; }

    string getVolcanicTextureLow() { return volcanicTextureLow; }
    void setVolcanicTextureLow(string inVolcanicTextureLow) { volcanicTextureLow = inVolcanicTextureLow; }
    string getVolcanicTextureMidFlat() { return volcanicTextureMidFlat; }
    void setVolcanicTextureMidFlat(string inVolcanicTextureMidFlat) { volcanicTextureMidFlat = inVolcanicTextureMidFlat; }
    string getVolcanicTextureMidSteep() { return volcanicTextureMidSteep; }
    void setVolcanicTextureMidSteep(string inVolcanicTextureMidSteep) { volcanicTextureMidSteep = inVolcanicTextureMidSteep; }
    string getVolcanicTextureHigh() { return volcanicTextureHigh; }
    void setVolcanicTextureHigh(string inVolcanicTextureHigh) { volcanicTextureHigh = inVolcanicTextureHigh; }

    string getTemperateTextureLow() { return temperateTextureLow; }
    void setTemperateTextureLow(string inTemperateTextureLow) { temperateTextureLow = inTemperateTextureLow; }
    string getTemperateTextureMidFlat() { return temperateTextureMidFlat; }
    void setTemperateTextureMidFlat(string inTemperateTextureMidFlat) { temperateTextureMidFlat = inTemperateTextureMidFlat; }
    string getTemperateTextureMidSteep() { return temperateTextureMidSteep; }
    void setTemperateTextureMidSteep(string inTemperateTextureMidSteep) { temperateTextureMidSteep = inTemperateTextureMidSteep; }
    string getTemperateTextureHigh() { return temperateTextureHigh; }
    void setTemperateTextureHigh(string inTemperateTextureHigh) { temperateTextureHigh = inTemperateTextureHigh; }

    string getSwampTextureLow() { return swampTextureLow; }
    void setSwampTextureLow(string inSwampTextureLow) { swampTextureLow = inSwampTextureLow; }
    string getSwampTextureMidFlat() { return swampTextureMidFlat; }
    void setSwampTextureMidFlat(string inSwampTextureMidFlat) { swampTextureMidFlat = inSwampTextureMidFlat; }
    string getSwampTextureMidSteep() { return swampTextureMidSteep; }
    void setSwampTextureMidSteep(string inSwampTextureMidSteep) { swampTextureMidSteep = inSwampTextureMidSteep; }
    string getSwampTextureHigh() { return swampTextureHigh; }
    void setSwampTextureHigh(string inSwampTextureHigh) { swampTextureHigh = inSwampTextureHigh; }

    string getSeasonalForestTextureLow() { return seasonalForestTextureLow; }
    void setSeasonalForestTextureLow(string inSeasonalForestTextureLow) { seasonalForestTextureLow = inSeasonalForestTextureLow; }
    string getSeasonalForestTextureMidFlat() { return seasonalForestTextureMidFlat; }
    void setSeasonalForestTextureMidFlat(string inSeasonalForestTextureMidFlat) { seasonalForestTextureMidFlat = inSeasonalForestTextureMidFlat; }
    string getSeasonalForestTextureMidSteep() { return seasonalForestTextureMidSteep; }
    void setSeasonalForestTextureMidSteep(string inSeasonalForestTextureMidSteep) { seasonalForestTextureMidSteep = inSeasonalForestTextureMidSteep; }
    string getSeasonalForestTextureHigh() { return seasonalForestTextureHigh; }
    void setSeasonalForestTextureHigh(string inSeasonalForestTextureHigh) { seasonalForestTextureHigh = inSeasonalForestTextureHigh; }

    string getAutumnTextureLow() { return autumnTextureLow; }
    void setAutumnTextureLow(string inAutumnTextureLow) { autumnTextureLow = inAutumnTextureLow; }
    string getAutumnTextureMidFlat() { return autumnTextureMidFlat; }
    void setAutumnTextureMidFlat(string inAutumnTextureMidFlat) { autumnTextureMidFlat = inAutumnTextureMidFlat; }
    string getAutumnTextureMidSteep() { return autumnTextureMidSteep; }
    void setAutumnTextureMidSteep(string inAutumnTextureMidSteep) { autumnTextureMidSteep = inAutumnTextureMidSteep; }
    string getAutumnTextureHigh() { return autumnTextureHigh; }
    void setAutumnTextureHigh(string inAutumnTextureHigh) { autumnTextureHigh = inAutumnTextureHigh; }

    string getMesaTextureLow() { return mesaTextureLow; }
    void setMesaTextureLow(string inMesaTextureLow) { mesaTextureLow = inMesaTextureLow; }
    string getMesaTextureMidFlat() { return mesaTextureMidFlat; }
    void setMesaTextureMidFlat(string inMesaTextureMidFlat) { mesaTextureMidFlat = inMesaTextureMidFlat; }
    string getMesaTextureMidSteep() { return mesaTextureMidSteep; }
    void setMesaTextureMidSteep(string inMesaTextureMidSteep) { mesaTextureMidSteep = inMesaTextureMidSteep; }
    string getMesaTextureHigh() { return mesaTextureHigh; }
    void setMesaTextureHigh(string inMesaTextureHigh) { mesaTextureHigh = inMesaTextureHigh; }

    string getHotDesertTextureLow() { return hotDesertTextureLow; }
    void setHotDesertTextureLow(string inHotDesertTextureLow) { hotDesertTextureLow = inHotDesertTextureLow; }
    string getHotDesertTextureMidFlat() { return hotDesertTextureMidFlat; }
    void setHotDesertTextureMidFlat(string inHotDesertTextureMidFlat) { hotDesertTextureMidFlat = inHotDesertTextureMidFlat; }
    string getHotDesertTextureMidSteep() { return hotDesertTextureMidSteep; }
    void setHotDesertTextureMidSteep(string inHotDesertTextureMidSteep) { hotDesertTextureMidSteep = inHotDesertTextureMidSteep; }
    string getHotDesertTextureHigh() { return hotDesertTextureHigh; }
    void setHotDesertTextureHigh(string inHotDesertTextureHigh) { hotDesertTextureHigh = inHotDesertTextureHigh; }

    string getDustyTextureLow() { return dustyTextureLow; }
    void setDustyTextureLow(string inDustyTextureLow) { dustyTextureLow = inDustyTextureLow; }
    string getDustyTextureMidFlat() { return dustyTextureMidFlat; }
    void setDustyTextureMidFlat(string inDustyTextureMidFlat) { dustyTextureMidFlat = inDustyTextureMidFlat; }
    string getDustyTextureMidSteep() { return dustyTextureMidSteep; }
    void setDustyTextureMidSteep(string inDustyTextureMidSteep) { dustyTextureMidSteep = inDustyTextureMidSteep; }
    string getDustyTextureHigh() { return dustyTextureHigh; }
    void setDustyTextureHigh(string inDustyTextureHigh) { dustyTextureHigh = inDustyTextureHigh; }

    string getBadlandsTextureLow() { return badlandsTextureLow; }
    void setBadlandsTextureLow(string inBadlandsTextureLow) { badlandsTextureLow = inBadlandsTextureLow; }
    string getBadlandsTextureMidFlat() { return badlandsTextureMidFlat; }
    void setBadlandsTextureMidFlat(string inBadlandsTextureMidFlat) { badlandsTextureMidFlat = inBadlandsTextureMidFlat; }
    string getBadlandsTextureMidSteep() { return badlandsTextureMidSteep; }
    void setBadlandsTextureMidSteep(string inBadlandsTextureMidSteep) { badlandsTextureMidSteep = inBadlandsTextureMidSteep; }
    string getBadlandsTextureHigh() { return badlandsTextureHigh; }
    void setBadlandsTextureHigh(string inBadlandsTextureHigh) { badlandsTextureHigh = inBadlandsTextureHigh; }

    string getOasisTextureLow() { return oasisTextureLow; }
    void setOasisTextureLow(string inOasisTextureLow) { oasisTextureLow = inOasisTextureLow; }
    string getOasisTextureMidFlat() { return oasisTextureMidFlat; }
    void setOasisTextureMidFlat(string inOasisTextureMidFlat) { oasisTextureMidFlat = inOasisTextureMidFlat; }
    string getOasisTextureMidSteep() { return oasisTextureMidSteep; }
    void setOasisTextureMidSteep(string inOasisTextureMidSteep) { oasisTextureMidSteep = inOasisTextureMidSteep; }
    string getOasisTextureHigh() { return oasisTextureHigh; }
    void setOasisTextureHigh(string inOasisTextureHigh) { oasisTextureHigh = inOasisTextureHigh; }

    string getOceanTextureLow() { return oceanTextureLow; }
    void setOceanTextureLow(string inOceanTextureLow) { oceanTextureLow = inOceanTextureLow; }
    string getOceanTextureMidFlat() { return oceanTextureMidFlat; }
    void setOceanTextureMidFlat(string inOceanTextureMidFlat) { oceanTextureMidFlat = inOceanTextureMidFlat; }
    string getOceanTextureMidSteep() { return oceanTextureMidSteep; }
    void setOceanTextureMidSteep(string inOceanTextureMidSteep) { oceanTextureMidSteep = inOceanTextureMidSteep; }
    string getOceanTextureHigh() { return oceanTextureHigh; }
    void setOceanTextureHigh(string inOceanTextureHigh) { oceanTextureHigh = inOceanTextureHigh; }

    string getCliffsTextureLow() { return cliffsTextureLow; }
    void setCliffsTextureLow(string inCliffsTextureLow) { cliffsTextureLow = inCliffsTextureLow; }
    string getCliffsTextureMidFlat() { return cliffsTextureMidFlat; }
    void setCliffsTextureMidFlat(string inCliffsTextureMidFlat) { cliffsTextureMidFlat = inCliffsTextureMidFlat; }
    string getCliffsTextureMidSteep() { return cliffsTextureMidSteep; }
    void setCliffsTextureMidSteep(string inCliffsTextureMidSteep) { cliffsTextureMidSteep = inCliffsTextureMidSteep; }
    string getCliffsTextureHigh() { return cliffsTextureHigh; }
    void setCliffsTextureHigh(string inCliffsTextureHigh) { cliffsTextureHigh = inCliffsTextureHigh; }
};

#endif
