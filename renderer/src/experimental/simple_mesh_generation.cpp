#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
    #include "/dcs/large/efogahlewem/.local/include/stb_image.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/type_ptr.hpp"
#else
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
    #include <stb_image.h>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>
#endif

float bilinear_interpolation(float x, float z, float **image, float x1, float x2, float z1, float z2) {
    float r1 = (x2 - x) / (x2 - x1) * image[static_cast<int>(z1)][static_cast<int>(x)] + (x - x1) / (x2 - x1) * image[static_cast<int>(z1)][static_cast<int>(x2)];
    float r2 = (x2 - x) / (x2 - x1) * image[static_cast<int>(z2)][static_cast<int>(x)] + (x - x1) / (x2 - x1) * image[static_cast<int>(z2)][static_cast<int>(x2)];
    return (z2 - z) / (z2 - z1) * r1 + (z - z1) / (z2 - z1) * r2;
    // Check if this can be optimized
}

float sgn(float x) {
    if (x > 0) {
        return 1.0;
    } else if (x < 0) {
        return -1.0;
    } else {
        return 0.0;
    }
}

// A function to read a heightmap from a RAW file and store it in a 2D array
// The heightmap is assumed to be a square
float **read_heightmap(const char *filename, int size) {
    FILE *file;
    file = fopen(filename, "rb");
    if (file == NULL) {
        std::cout << "Error: Could not open file" << std::endl;
        return NULL;
    }
    float **heightmap = new float*[size];
    for (int i = 0; i < size; i++) {
        heightmap[i] = new float[size];
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            // read the next float from the file
            float height;
            fread(&height, sizeof(float), 1, file);
            heightmap[i][j] = height;
        }
    }
    fclose(file);
    return heightmap;
}

float height_scaling(float value, int type, float scale_factor, float function_factor, float blend_factor=0.5) {
    switch (type)
    {
    case 0:
        // This is the linear smoothing function
        // We are computing v * sf
        return value * scale_factor;
    case 1:
        // This is the power smoothing function
        // We are computing sgn(v) * |v|^f * sf
        return sgn(value) * pow(abs(value), function_factor) * scale_factor;
    case 2:
        // This is sigmoid smoothing function using tanh
        // We are computing sf * tanh(f * v) / tanh(f)
        return scale_factor * (tanh(function_factor * value) / tanh(function_factor));
    case 3:
        // This is logarithmic smoothing function
        // We are computing sgn(v) * log(1 + f * ((v + 1) / 2)) / log(1 + f)
        return sgn(value) * (log(1 + function_factor * ((value + 1)/ 2))/ log(1+ function_factor));
    case 4:
        // This is a hybrid approach between linear and sigmoid smoothing
        // We are computing (1 - bf) * v * sf + bf * (sf * tanh(f * v) / tanh(f))
        return (1-blend_factor) * value * scale_factor + blend_factor * (scale_factor * (tanh(function_factor * value) / tanh(function_factor)));
    default:
        // There is no scaling applied
        // We are computing v
        return value;
    }
}

// A function to generate a mesh from a heightmap using bilinear interpolation where necessary if the resolution of the
// mesh is higher than the resolution of the heightmap
// Size is the size of the heightmap and resolution is the resolution of the mesh

std::vector<std::vector<glm::vec3>> generate_mesh(float **heightmap, int size, int resolution) {
    // We want to scale the heightmap values from the range [0,1] to the range [0, 64]
    float function_factor = 0.5;
    float scaling_factor = 96.0;
    std::vector<std::vector<glm::vec3>> mesh = std::vector<std::vector<glm::vec3>>(resolution, std::vector<glm::vec3>(resolution));

    float step = static_cast<float>(size) / static_cast<float>(resolution);

    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            float x = i * step;
            float z = j * step;
            float x1 = static_cast<int>(x);
            float x2 = x1 + 1;
            float z1 = static_cast<int>(z);
            float z2 = z1 + 1;
            if ((x2 >= size || z2 >= size) || (x1 == x && z1 == z && x2 == x + 1 && z2 == z + 1)) {
                mesh[j][i] = glm::vec3(x, height_scaling(heightmap[static_cast<int>(z)][static_cast<int>(x)], 0, scaling_factor, function_factor), z);
            } else {
                mesh[j][i] = glm::vec3(x, height_scaling(bilinear_interpolation(x, z, heightmap, x1, x2, z1, z2), 0, scaling_factor, function_factor), z);
            }
        }
    }

    // for (int i = 0; i < resolution; i++) {
    //     for (int j = 0; j < resolution; j++) {
    //         float x = j * step;
    //         float z = i * step;
    //         float x1 = static_cast<int>(x);
    //         float x2 = static_cast<int>(x) + 1;
    //         float z1 = static_cast<int>(z);
    //         float z2 = static_cast<int>(z) + 1;
    //         if ((x2 >= size || z2 >= size) || (x1 == x && z1 == z && x2 == x + 1 && z2 == z + 1)) {
    //             mesh[i][j] = glm::vec3(x, height_scaling(heightmap[static_cast<int>(z)][static_cast<int>(x)], 0, scaling_factor, function_factor), z);
    //         } else {
    //             mesh[i][j] = glm::vec3(x, height_scaling(bilinear_interpolation(x, z, heightmap, x1, x2, z1, z2), 0, scaling_factor, function_factor), z);
    //         }
    //     }
    // }

    return mesh;
}

// This produced our flattened mesh which can be the vertex buffer for OpenGL
std::vector<glm::vec3> flatten_mesh(std::vector<std::vector<glm::vec3>> mesh, int resolution) {
    std::vector<glm::vec3> flattened_mesh = std::vector<glm::vec3>(resolution * resolution);
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            flattened_mesh[i * resolution + j] = mesh[i][j];
        }
    }
    return flattened_mesh;
}

// We need to generate the index buffer for OpenGL to render the mesh
// Each quad will have 2 triangles and each triangle will have 3 vertices
// The widing order of the triangles is important and is clockwise
std::vector<unsigned int> generate_index_buffer(int resolution) {
    std::vector<unsigned int> index_buffer = std::vector<unsigned int>((resolution - 1) * (resolution - 1) * 2 * 3);
    for (int i = 0; i < resolution - 1; i++) {
        for (int j = 0; j < resolution - 1; j++) {
            // First triangle of the form [i,j], [i+1, j+1], [i+1, j]
            index_buffer[i * (resolution - 1) * 6 + j * 6] = i * resolution + j;
            index_buffer[i * (resolution - 1) * 6 + j * 6 + 1] = (i + 1) * resolution + j + 1;
            index_buffer[i * (resolution - 1) * 6 + j * 6 + 2] = (i + 1) * resolution + j;

            // Second triangle of the form [i,j], [i, j+1], [i+1, j+1]
            index_buffer[i * (resolution - 1) * 6 + j * 6 + 3] = i * resolution + j;
            index_buffer[i * (resolution - 1) * 6 + j * 6 + 4] = i * resolution + j + 1;
            index_buffer[i * (resolution - 1) * 6 + j * 6 + 5] = (i + 1) * resolution + j + 1;
        }
    }
    return index_buffer;
}

glm::vec3 compute_normal_contribution(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    glm::vec3 edge1 = v2 - v1;
    glm::vec3 edge2 = v3 - v1;
    return glm::cross(edge1, edge2);
}

std::vector<std::vector<glm::vec3>> generate_vertices_normals(std::vector<std::vector<glm::vec3>> mesh, int resolution) {
    std::vector<std::vector<glm::vec3>> vertices_normals = std::vector<std::vector<glm::vec3>>(resolution, std::vector<glm::vec3>(resolution));
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            // The corner cases of [0,0]
            if ((i == 0 && j == 0)) {
                glm::vec3 A = mesh[i+1][j];
                glm::vec3 B = mesh[i+1][j+1];
                glm::vec3 C = mesh[i][j];
                glm::vec3 face2Norm = compute_normal_contribution(A, B, C);
                A = mesh[i][j+1];
                B = mesh[i][j];
                C = mesh[i+1][j+1];
                glm::vec3 face3Norm = compute_normal_contribution(A, B, C);
                vertices_normals[i][j] = glm::normalize(face2Norm + face3Norm);
            // The corner case of [0, resolution - 1]
            } else if (i == 0 && j == resolution - 1) {
                glm::vec3 A = mesh[i][j];
                glm::vec3 B = mesh[i][j-1];
                glm::vec3 C = mesh[i+1][j];
                glm::vec3 face1Norm = compute_normal_contribution(A, B, C);
                vertices_normals[i][j] = glm::normalize(face1Norm);
            // The corner case of [resolution - 1, 0]
            } else if (i == resolution - 1 && j == 0) {
                glm::vec3 A = mesh[i][j];
                glm::vec3 B = mesh[i][j+1];
                glm::vec3 C = mesh[i-1][j];
                glm::vec3 face4Norm = compute_normal_contribution(A, B, C);
                vertices_normals[i][j] = glm::normalize(face4Norm);
            // The corner case of [resolution - 1, resolution - 1]
            } else if (i == resolution - 1 && j == resolution - 1) {
                glm::vec3  A = mesh[i-1][j];
                glm::vec3  B = mesh[i-1][j-1];
                glm::vec3  C = mesh[i][j];
                glm::vec3 face5Norm = compute_normal_contribution(A, B, C);
                A = mesh[i][j-1];
                B = mesh[i][j];
                C = mesh[i-1][j-1];
                glm::vec3 face6Norm = compute_normal_contribution(A, B, C);
                vertices_normals[i][j] = glm::normalize(face5Norm + face6Norm);
            // The edge cases of the bottom row
            } else if (i == 0) {
                glm::vec3 A = mesh[i][j];
                glm::vec3 B = mesh[i][j-1];
                glm::vec3 C = mesh[i+1][j];
                glm::vec3 face1Norm = compute_normal_contribution(A, B, C);
                A = mesh[i+1][j];
                B = mesh[i+1][j+1];
                C = mesh[i][j];
                glm::vec3 face2Norm = compute_normal_contribution(A, B, C);
                A = mesh[i][j+1];
                B = mesh[i][j];
                C = mesh[i+1][j+1];
                glm::vec3 face3Norm = compute_normal_contribution(A, B, C);
                vertices_normals[i][j] = glm::normalize(face1Norm + face2Norm + face3Norm);
            // The edge cases of the left row
            } else if (j == 0) {
                glm::vec3 A = mesh[i+1][j];
                glm::vec3 B = mesh[i+1][j+1];
                glm::vec3 C = mesh[i][j];
                glm::vec3 face2Norm = compute_normal_contribution(A, B, C);
                A = mesh[i][j+1];
                B = mesh[i][j];
                C = mesh[i+1][j+1];
                glm::vec3 face3Norm = compute_normal_contribution(A, B, C);
                A = mesh[i][j];
                B = mesh[i][j+1];
                C = mesh[i-1][j];
                glm::vec3 face4Norm = compute_normal_contribution(A, B, C);
                vertices_normals[i][j] = glm::normalize(face2Norm + face3Norm + face4Norm);
            // The edge cases of the top row
            } else if (i == resolution - 1) {
                glm::vec3 A = mesh[i][j];
                glm::vec3 B = mesh[i][j+1];
                glm::vec3 C = mesh[i-1][j];
                glm::vec3 face4Norm = compute_normal_contribution(A, B, C);
                A = mesh[i-1][j];
                B = mesh[i-1][j-1];
                C = mesh[i][j];
                glm::vec3 face5Norm = compute_normal_contribution(A, B, C);
                A = mesh[i][j-1];
                B = mesh[i][j];
                C = mesh[i-1][j-1];
                glm::vec3 face6Norm = compute_normal_contribution(A, B, C);
                vertices_normals[i][j] = glm::normalize(face4Norm + face5Norm + face6Norm);
            // The edge cases of the right row
            } else if (j == resolution - 1) {
                glm::vec3 A = mesh[i][j];
                glm::vec3 B = mesh[i][j-1];
                glm::vec3 C = mesh[i+1][j];
                glm::vec3 face1Norm = compute_normal_contribution(A, B, C);
                A = mesh[i-1][j];
                B = mesh[i-1][j-1];
                C = mesh[i][j];
                glm::vec3 face5Norm = compute_normal_contribution(A, B, C);
                A = mesh[i][j-1];
                B = mesh[i][j];
                C = mesh[i-1][j-1];
                glm::vec3 face6Norm = compute_normal_contribution(A, B, C);
                vertices_normals[i][j] = glm::normalize(face1Norm + face5Norm + face6Norm);
            // The general case
            } else {
                glm::vec3 A = mesh[i][j];
                glm::vec3 B = mesh[i][j-1];
                glm::vec3 C = mesh[i+1][j];
                glm::vec3 face1Norm = compute_normal_contribution(A, B, C);
                A = mesh[i+1][j];
                B = mesh[i+1][j+1];
                C = mesh[i][j];
                glm::vec3 face2Norm = compute_normal_contribution(A, B, C);
                A = mesh[i][j+1];
                B = mesh[i][j];
                C = mesh[i+1][j+1];
                glm::vec3 face3Norm = compute_normal_contribution(A, B, C);
                A = mesh[i][j];
                B = mesh[i][j+1];
                C = mesh[i-1][j];
                glm::vec3 face4Norm = compute_normal_contribution(A, B, C);
                A = mesh[i-1][j];
                B = mesh[i-1][j-1];
                C = mesh[i][j];
                glm::vec3 face5Norm = compute_normal_contribution(A, B, C);
                A = mesh[i][j-1];
                B = mesh[i][j];
                C = mesh[i-1][j-1];
                glm::vec3 face6Norm = compute_normal_contribution(A, B, C);
                vertices_normals[i][j] = glm::normalize(face1Norm + face2Norm + face3Norm + face4Norm + face5Norm + face6Norm);
            }
        }
    }
    return vertices_normals;
}

std::vector<glm::vec3> flatten_normals(std::vector<std::vector<glm::vec3>> normals, int resolution) {
    std::vector<glm::vec3> flattened_normals = std::vector<glm::vec3>(resolution * resolution);
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            flattened_normals[i * resolution + j] = normals[i][j];
        }
    }
    return flattened_normals;
}

void storeToObj(std::vector<glm::vec3> vertices, std::vector<unsigned> indices, std::vector<glm::vec3> normals, int resolution, int indexBufferSize, std::string path) {
    std::ofstream objFile;
    objFile.open(path);
    objFile << "# OBJ file" << std::endl;
    objFile << "# Vertices" << std::endl;
    for (int i = 0; i < resolution * resolution; i++) {
        objFile << "v " << vertices[i][0] << " " << vertices[i][1] << " " << vertices[i][2] << std::endl;
    }
    objFile << "# Vertex Normals" << std::endl;
    for (int i = 0; i < resolution * resolution; i++) {
        objFile << "vn " << normals[i][0] << " " << normals[i][1] << " " << normals[i][2] << std::endl;
    }
    // The indices are 0-indexed and need to be converted to 1-indexed
    // The vertex entries will pair up with the corresponding vertex normal entries
    objFile << "# Faces (vertex // vertex normal)" << std::endl;
    for (int i = 0; i < indexBufferSize * 3; i += 3) {
        // We need to ensure that the indices are not written in scientific notation
        objFile << "f " << indices[i] + 1 << "//" << indices[i] + 1 << " " << indices[i + 1] + 1 << "//" << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << "//" << indices[i + 2] + 1 << std::endl;
    }
    objFile.close();
}

// TODO: Implement a function to load the obj and render it using OpenGL

int main(void) {
    // Read the environment variable $PROJECT_ROOT
    std::string project_root = std::getenv("PROJECT_ROOT");
    // Read the heightmap
    std::string filename = project_root + "/data/noise_coast_map.raw";
    int heightmapSize = 1024;
    int meshResolution = 1024;
    float **heightmap = read_heightmap(filename.c_str(), heightmapSize);
    // Print the heightmap
    // for (int i = 0; i < heightmapSize; i++) {
    //     for (int j = 0; j < heightmapSize; j++) {
    //         std::cout << heightmap[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << "First few values: " << std::endl;
    // std::cout << heightmap[0][0] << std::endl;
    // std::cout << heightmap[0][1] << std::endl;
    // std::cout << heightmap[1][0] << std::endl;

    // Generate the mesh
    std::vector<std::vector<glm::vec3>> mesh = generate_mesh(heightmap, heightmapSize, meshResolution);
    // Print the first few values
    // std::cout << "First few values: " << std::endl;
    // std::cout << mesh[0][0][0] << " " << mesh[0][0][1] << " " << mesh[0][0][2] << std::endl;
    // std::cout << mesh[0][1][0] << " " << mesh[0][1][1] << " " << mesh[0][1][2] << std::endl;
    // std::cout << mesh[0][2][0] << " " << mesh[0][2][1] << " " << mesh[0][2][2] << std::endl;
    // std::cout << mesh[1][0][0] << " " << mesh[1][0][1] << " " << mesh[1][0][2] << std::endl;

    // Flatten the mesh
    std::vector<glm::vec3> flattened_mesh = flatten_mesh(mesh, meshResolution);
    std::cout << "Flattened mesh size: " << flattened_mesh.size() << std::endl;
    // Print the first few values
    // std::cout << "First few values: " << std::endl;
    // std::cout << flattened_mesh[0][0] << " " << flattened_mesh[0][1] << " " << flattened_mesh[0][2] << std::endl;
    // std::cout << flattened_mesh[1][0] << " " << flattened_mesh[1][1] << " " << flattened_mesh[1][2] << std::endl;
    // std::cout << flattened_mesh[2][0] << " " << flattened_mesh[2][1] << " " << flattened_mesh[2][2] << std::endl;
    // std::cout << flattened_mesh[1024][0] << " " << flattened_mesh[1024][1] << " " << flattened_mesh[1024][2] << std::endl;

    std::vector<unsigned int> index_buffer = generate_index_buffer(meshResolution);
    std::cout << "Index buffer size: " << index_buffer.size() << std::endl;
    // Print the first few values
    std::cout << "First few values: " << std::endl;
    std::cout << index_buffer[0] << " " << index_buffer[1] << " " << index_buffer[2] << std::endl;
    std::cout << index_buffer[3] << " " << index_buffer[4] << " " << index_buffer[5] << std::endl;
    std::cout << index_buffer[6] << " " << index_buffer[7] << " " << index_buffer[8] << std::endl;
    std::cout << index_buffer[9] << " " << index_buffer[10] << " " << index_buffer[11] << std::endl;

    std::vector<std::vector<glm::vec3>> vertices_normals = generate_vertices_normals(mesh, meshResolution);
    std::vector<glm::vec3> flattened_normals = flatten_normals(vertices_normals, meshResolution);
    std::cout << "Flattened normals size: " << flattened_normals.size() << std::endl;

    int indexBufferSize = (meshResolution - 1) * (meshResolution - 1) * 2;
    std::string path = project_root + "/data/noise_coast_map1.obj";
    storeToObj(flattened_mesh, index_buffer, flattened_normals, meshResolution, indexBufferSize, path);
    return 0;
}