/**
 * @file Utility.hpp
 * @author King Attalus II
 * @brief This file contains the Utility class, which is used to perform various utility functions for the application.
 * @version 1.0
 * @date 2025
 *
 */
#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <vector>
#include <optional>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

using namespace std;

/**
 * @brief This class contains various utility functions for the application.
 *
 * @details The Utility class contains static functions that can be used throughout the application for various
 * calculations and operations. These include linear interpolation, bilinear interpolation, cubic interpolation,
 * height scaling, and reading/writing heightmaps to/from files.
 *
 */
class Utility
{
private:
    // The constructor is private as we do not want to instantiate this class
    inline Utility(){};
    inline ~Utility(){};
public:
    static float lerp(float x, float x1, float x2, float y1, float y2);
    static float lerp(float x, glm::vec2 p1, glm::vec2 p2);
    static float sgn(float x);
    static float bilinear_interpolation(
        float x,
        float z,
        float **heightmap,
        float x1,
        float x2,
        float z1,
        float z2
    );
    static float bilinear_interpolation(
        glm::vec2 position,
        float **heightmap,
        glm::vec2 bottomLeft,
        glm::vec2 topRight
    );
    static float bilinear_interpolation(
        glm::vec2 position,
        glm::vec3 bottomLeft,
        glm::vec3 bottomRight,
        glm::vec3 topLeft,
        glm::vec3 topRight
    );
    // Write a function prototype for bicubic interpolation
    static float bicubic_interpolation(
        glm::vec2 position,
        vector<vector<float>> heightmap
    );
    static float cubic_interpolation(
        float p0,
        float p1,
        float p2,
        float p3,
        float t
    );
    static float height_scaling(float height, float scale_factor);
    // Returns an optional vector incase the file could not be opened
    static optional<vector<vector<float>>> readHeightmap(const char *filename, int size);
    // We pass an optional vector of normals in case we do not have them
    static void storeHeightmapToObj(
        const char *filename,
        vector<glm::vec3> vertices,
        optional<vector<glm::vec3>> normals,
        vector<unsigned int> indices
    );
};

#endif // UTILITY_HPP
