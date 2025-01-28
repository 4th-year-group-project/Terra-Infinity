/*
    This is a class that will contain many different static utility methods that can be used
    throughout the project to perform various tasks.
*/
#include <vector>
#include <iostream>
#include <fstream>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include <Utility.hpp>

using namespace std;

float Utility::lerp(float x, float x1, float x2, float y1, float y2) {
    return y1 + (y2 - y1) * ((x - x1) / (x2 - x1));
}

float Utility::lerp(float x, glm::vec2 p1, glm::vec2 p2) {
    return p1.y + (p2.y - p1.y) * ((x - p1.x) / (p2.x - p1.x));
}

float Utility::sgn(float x) {
    return (x > 0) ? 1.0 : (x < 0) ? -1.0 : 0.0;
}

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

float Utility::bilinear_interpolation(
    glm::vec2 position,
    glm::vec3 bottomLeft,
    glm::vec3 bottomRight,
    glm::vec3 topLeft,
    glm::vec3 topRight
) {
    float multaplicative_constant = 1.0 / ((topRight.x - bottomLeft.x) * (topRight.z - bottomLeft.z));
    glm::vec2 row_vector = glm::vec2(topRight.x - position.x, position.x - bottomLeft.x);
    glm::vec2 column_vector = glm::vec2(topRight.z - position.y, position.y - bottomLeft.z);
    glm::mat2x2 matrix = glm::mat2x2(
        bottomLeft.y, topLeft.y,
        bottomRight.y, topRight.y
    );
    return multaplicative_constant * glm::dot(row_vector, matrix * column_vector);
}

float Utility::height_scaling(float height, float scale_factor) {
    return height * scale_factor;
}

optional<vector<glm::vec3>> Utility::readHeightmap(const char *filename, int size) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        cout << "Error: Could not open file" << endl;
        return nullopt;
    }
    vector<glm::vec3> heightmap;
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            float height;
            fread(&height, sizeof(float), 1, file);
            heightmap.push_back(glm::vec3(x, height, z));
        }
    }
    fclose(file);
    return heightmap;
}

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
    for (unsigned int index : indices) {
        objFile << "f " << index << endl;
    }
    objFile.close();
}

