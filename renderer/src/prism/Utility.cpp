/**
 * @file Utility.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Utility class.
 * @details This class contains many different static utility methods that can be used
 * throughout the project to perform various tasks.
 * @version 1.0
 * @date 2025
 * 
 */
#include <vector>
#include <iostream>
#include <fstream>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "Utility.hpp"

using namespace std;

/**
 * @brief This function will complete linear interpolation between two points
 * 
 * @param x [in] float The x value to interpolate
 * @param x1 [in] float The x value of the first point
 * @param x2 [in] float The x value of the second point
 * @param y1 [in] float The y value of the first point
 * @param y2 [in] float The y value of the second point
 * 
 * @return float The interpolated y value at the given x value
 * 
 */
float Utility::lerp(float x, float x1, float x2, float y1, float y2) {
    return y1 + (y2 - y1) * ((x - x1) / (x2 - x1));
}

/**
 * @brief This function will complete linear interpolation between two points
 * 
 * @param x [in] float The x value to interpolate
 * @param p1 [in] glm::vec2 The first point
 * @param p2 [in] glm::vec2 The second point
 * 
 * @return float The interpolated y value at the given x value
 * 
 */
float Utility::lerp(float x, glm::vec2 p1, glm::vec2 p2) {
    return p1.y + (p2.y - p1.y) * ((x - p1.x) / (p2.x - p1.x));
}

/**
 * @brief This function will compute the sign of a number
 * 
 * @param x [in] float The number to compute the sign of
 * 
 * @return float The sign of the number
 * 
 */
float Utility::sgn(float x) {
    return (x > 0) ? 1.0 : (x < 0) ? -1.0 : 0.0;
}

/**
 * @brief This function will compute the bilinear interpolation of between four points
 * 
 * @param x [in] float The x value to interpolate
 * @param z [in] float The z value to interpolate
 * @param heightmap [in] float** The heightmap to interpolate from
 * @param x1 [in] float The x value of the first point
 * @param x2 [in] float The x value of the second point
 * @param z1 [in] float The z value of the first point
 * @param z2 [in] float The z value of the second point
 * 
 * @return float The interpolated y value at the given x and z values
 * 
 */
float Utility::bilinear_interpolation(
    float x,
    float z,
    float **heightmap,
    float x1,
    float x2,
    float z1,
    float z2
) {
    // This function is implemented with the linear algebra formula for bilinear interpolation
    // value = 1 / ((x2 - x1)*(z2-z1)) *[x2-x   x-x1] [f(Q_x1z1)   f(Q_x1z2) \\ f(Q_x2z1)   f(Q_x2z2)][z2-z \\ z-z1]

    int i_x1 = static_cast<int>(x1);
    int i_x2 = static_cast<int>(x2);
    int i_z1 = static_cast<int>(z1);
    int i_z2 = static_cast<int>(z2);
    float multaplicative_constant = 1.0 / ((x2 - x1) * (z2 - z1));
    glm::vec2 row_vector = glm::vec2(x2 - x, x - x1);
    glm::vec2 column_vector = glm::vec2(z2 - z, z - z1);
    glm::mat2x2 matrix = glm::mat2x2(
        heightmap[i_z1][i_x1], heightmap[i_z2][i_x1],
        heightmap[i_z1][i_x2], heightmap[i_z2][i_x2]
    );
    return multaplicative_constant * glm::dot(row_vector, matrix * column_vector);
}

/**
 * @brief This function will compute the bilinear interpolation of between four points
 * 
 * @param position [in] glm::vec2 The position to interpolate
 * @param heightmap [in] float** The heightmap to interpolate from
 * @param bottomLeft [in] glm::vec2 The bottom left point
 * @param topRight [in] glm::vec2 The top right point
 * 
 * @return float The interpolated y value at the given x and z values
 * 
 */
float Utility::bilinear_interpolation(
    glm::vec2 position,
    float **heightmap,
    glm::vec2 bottomLeft,
    glm::vec2 topRight
) {
    int i_x1 = static_cast<int>(bottomLeft.x);
    int i_x2 = static_cast<int>(topRight.x);
    int i_z1 = static_cast<int>(bottomLeft.y);
    int i_z2 = static_cast<int>(topRight.y);

    float multaplicative_constant = 1.0 / ((topRight.x - bottomLeft.x) * (topRight.y - bottomLeft.y));
    glm::vec2 row_vector = glm::vec2(topRight.x - position.x, position.x - bottomLeft.x);
    glm::vec2 column_vector = glm::vec2(topRight.y - position.y, position.y - bottomLeft.y);
    glm::mat2x2 matrix = glm::mat2x2(
        heightmap[i_z1][i_x1], heightmap[i_z2][i_x1],
        heightmap[i_z1][i_x2], heightmap[i_z2][i_x2]
    );
    return multaplicative_constant * glm::dot(row_vector, matrix * column_vector);
}

/**
 * @brief This function will compute the bilinear interpolation of between four points
 * 
 * @param position [in] glm::vec2 The position to interpolate
 * @param bottomLeft [in] glm::vec3 The bottom left point
 * @param bottomRight [in] glm::vec3 The bottom right point
 * @param topLeft [in] glm::vec3 The top left point
 * @param topRight [in] glm::vec3 The top right point
 * 
 * @return float The interpolated y value at the given x and z values
 * 
 */
float Utility::bilinear_interpolation(
    glm::vec2 position,
    glm::vec3 bottomLeft,
    glm::vec3 bottomRight,
    glm::vec3 topLeft,
    glm::vec3 topRight
) {
    // bottom left (x1, z1)
    // bottom right (x2, z1)
    // top left (x1, z2)
    // top right (x2, z2)
    float x1 = bottomLeft.x;
    float x2 = topRight.x;
    float z1 = bottomLeft.z;
    float z2 = topRight.z;
    float x = position.x;
    float z = position.y;
    float fracBottom = (x2 - x1) * (z2 - z1);

    float firstTerm = (((x2 - x) * (z2 - z)) / fracBottom) * bottomLeft.y;
    float secondTerm = (((x - x1) * (z2 - z)) / fracBottom) * bottomRight.y;
    float thirdTerm = (((x2 - x) * (z - z1) / fracBottom)) * topLeft.y;
    float fourthTerm = (((x - x1) * (z - z1) / fracBottom)) * topRight.y;

    return firstTerm + secondTerm + thirdTerm + fourthTerm;


    // float multaplicative_constant = 1.0 / ((topRight.x - bottomLeft.x) * (topRight.z - bottomLeft.z));
    // glm::vec2 row_vector = glm::vec2(topRight.x - position.x, position.x - bottomLeft.x);
    // glm::vec2 column_vector = glm::vec2(topRight.z - position.y, position.y - bottomLeft.z);
    // glm::mat2x2 matrix = glm::mat2x2(
    //     bottomLeft.y, topLeft.y,
    //     bottomRight.y, topRight.y
    // );
    // return multaplicative_constant * glm::dot(row_vector, matrix * column_vector);
}

/**
 * @brief This function will compute the cubic interpolation of between four points
 * 
 * @param p0 [in] float The first point
 * @param p1 [in] float The second point
 * @param p2 [in] float The third point
 * @param p3 [in] float The fourth point
 * @param t [in] float The t value to interpolate
 * 
 * @return float The interpolated y value at the given t value
 * 
 */
float Utility::cubic_interpolation(
    float p0,
    float p1,
    float p2,
    float p3,
    float t
) {
    return p1 + 0.5f * t * (p2 - p0 + t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3 + t * (3.0f * (p1 - p2) + p3 - p0)));
}

/**
 * @brief This function will compute the bicubic interpolation of between sixteen points
 * 
 * @param position [in] glm::vec2 The position to interpolate
 * @param heightmap [in] vector<vector<float>> The heightmap to interpolate from
 * 
 * @return float The interpolated y value at the given x and z values
 * 
 */
float Utility::bicubic_interpolation(
    glm::vec2 position,
    vector<vector<float>> heightmap
) {
    // We are implementing the bicubic interpolation algorithm to better improve the quality
    // of the terrain mesh between the heightmap specified vertices
    
    int width = heightmap.size();
    int height = heightmap[0].size();
    
    // Get the integer coordinates and fractional parts
    int x = static_cast<int>(position.x);
    int z = static_cast<int>(position.y);
    float tx = position.x - x;
    float tz = position.y - z;
    
    // Perform bicubic interpolation using a 4x4 grid of points
    float y[4];
    
    // Interpolate in the x direction for each row of the 4x4 grid
    for (int j = 0; j < 4; j++) {
        float p[4];
        for (int i = 0; i < 4; i++) {
            // Apply clamping to ensure we stay within the heightmap bounds
            int ix = max(0, min(width - 1, x + i - 1));
            int jz = max(0, min(height - 1, z + j - 1));
            p[i] = heightmap[jz][ix];
        }
        // Interpolate along this row
        y[j] = cubic_interpolation(p[0], p[1], p[2], p[3], tx);
    }
    // Interpolate the results in the z direction
    return cubic_interpolation(y[0], y[1], y[2], y[3], tz);
}
    // of the terrain mesh between the heightmap specified vertices
    // We need to calculate the 16 control points for the bicubic interpolation

/**
 * @brief This function will compute the height scaling of a given height
 * 
 * @param height [in] float The height to scale
 * @param scale_factor [in] float The scale factor to apply
 * @return float The scaled height
 */
float Utility::height_scaling(float height, float scale_factor) {
    return height * scale_factor;
}

/**
 * @brief This function will read a heightmap from a file
 * 
 * @param filename [in] const char* The name of the file to read
 * @param size [in] int The size of the heightmap
 * 
 * @return std::optional<std::vector<std::vector<float>>> The heightmap read from the file
 */
optional<vector<vector<float>>> Utility::readHeightmap(const char *filename, int size) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        cout << "Error: Could not open file" << endl;
        return nullopt;
    }
    vector<vector<float>> heightmap2D = vector<vector<float>>(size, vector<float>(size));
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            float height;
            if (fread(&height, sizeof(float), 1, file) != 1) {
                std::cerr << "Error reading float from file at (" << x << ", " << z << ")\n";
                fclose(file);
                return std::nullopt;
            }
            heightmap2D[x][z] = height;
        }
    }
    return heightmap2D;
}

/**
 * @brief This function will write a heightmap to an obj file
 * 
 * @param filename [in] const char* The name of the file to write
 * @param vertices [in] vector<glm::vec3> The vertices to write
 * @param normals [in] std::optional<vector<glm::vec3>> The normals to write
 * @param indices [in] vector<unsigned int> The indices to write
 * 
 * @return void
 *
 */  
void Utility::storeHeightmapToObj(
    const char *filename,
    vector<glm::vec3> vertices,
    optional<vector<glm::vec3>> normals,
    vector<unsigned int> indices
) {

    ofstream objFile;
    objFile.open(filename);
    objFile << "# OBJ file" << endl;
    objFile << "# Vertices" << endl;
    for (glm::vec3 vertex : vertices) {
        objFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << endl;
    }
    if (normals.has_value()) {
        objFile << "# Normals" << endl;
        for (glm::vec3 normal : normals.value()) {
            objFile << "vn " << normal.x << " " << normal.y << " " << normal.z << endl;
        }
    }
    objFile << "# Faces (vertex // vertex normal)" << endl;
    if (normals.has_value()) {
        for(int i = 0; i < static_cast<int>(indices.size()); i += 3) {
            objFile << "f " << indices[i] + 1 << "//" << indices[i] + 1 << " " << indices[i + 1] + 1 << "//" << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << "//" << indices[i + 2]+ 1  << endl;
        }
    } else {
        for (int i = 0; i < static_cast<int>(indices.size()); i += 3) {
            objFile << "f " << indices[i] + 1 << " " << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << endl;
        }
    }
    objFile.close();
}

