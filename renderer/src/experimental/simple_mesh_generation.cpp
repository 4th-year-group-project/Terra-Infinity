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

double bilinear_interpolation(double x, double z, double **image, double x1, double x2, double z1, double z2) {
    double r1 = (x2 - x) / (x2 - x1) * image[static_cast<int>(z1)][static_cast<int>(x)] + (x - x1) / (x2 - x1) * image[static_cast<int>(z1)][static_cast<int>(x2)];
    double r2 = (x2 - x) / (x2 - x1) * image[static_cast<int>(z2)][static_cast<int>(x)] + (x - x1) / (x2 - x1) * image[static_cast<int>(z2)][static_cast<int>(x2)];
    return (z2 - z) / (z2 - z1) * r1 + (z - z1) / (z2 - z1) * r2;
    // Check if this can be optimized
}

double sgn(double x) {
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
double **read_heightmap(const char *filename, int size) {
    FILE *file;
    file = fopen(filename, "rb");
    if (file == NULL) {
        std::cout << "Error: Could not open file" << std::endl;
        return NULL;
    }
    double **heightmap = new double*[size];
    for (int i = 0; i < size; i++) {
        heightmap[i] = new double[size];
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            // read the next double from the file
            double height;
            fread(&height, sizeof(double), 1, file);
            heightmap[i][j] = height;
        }
    }
    fclose(file);
    return heightmap;
}

double height_scaling(double value, int type, double scale_factor, double function_factor, double blend_factor=0.5) {
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


std::vector<std::vector<glm::vec3>> generate_mesh(double **heightmap, int size, int resolution) {
    // We want to scale the heightmap values from the range [0,1] to the range [0, 64]
    double function_factor = 0.5;
    double scaling_factor = 64.0;
    std::vector<std::vector<glm::vec3>> mesh = std::vector<std::vector<glm::vec3>>(resolution, std::vector<glm::vec3>(resolution));

    double step = static_cast<double>(size) / static_cast<double>(resolution);

    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            double x = j * step;
            double z = i * step;
            double x1 = static_cast<int>(x);
            double x2 = static_cast<int>(x) + 1;
            double z1 = static_cast<int>(z);
            double z2 = static_cast<int>(z) + 1;
            if ((x2 >= size || z2 >= size) || (x1 == x && z1 == z && x2 == x + 1 && z2 == z + 1)) {
                mesh[i][j] = glm::vec3(x, height_scaling(heightmap[static_cast<int>(z)][static_cast<int>(x)], 0, scaling_factor, function_factor), z);
            } else {
                mesh[i][j] = glm::vec3(x, height_scaling(bilinear_interpolation(x, z, heightmap, x1, x2, z1, z2), 0, scaling_factor, function_factor), z);
            }
        }
    }
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
std::vector<glm::vec3> generate_index_buffer(int resolution) {
    std::vector<glm::vec3> index_buffer = std::vector<glm::vec3>((resolution - 1) * (resolution - 1) * 2);
    for (int i = 0; i < resolution - 1; i++) {
        for (int j = 0; j < resolution - 1; j++) {
            // First triangle
            index_buffer[i * (resolution - 1) * 2 + j * 2] = glm::vec3(i * resolution + j, i * resolution + j + 1, (i + 1) * resolution + j);
            // Second triangle
            index_buffer[i * (resolution - 1) * 2 + j * 2 + 1] = glm::vec3(i * resolution + j + 1, (i + 1) * resolution + j + 1, (i + 1) * resolution + j);
        }
    }
    return index_buffer;
}


std::vector<std::vector<glm::vec3>> generate_vertices_normals(std::vector<std::vector<glm::vec3>> mesh, int resolution) {
    std::vector<std::vector<glm::vec3>> vertices_normals = std::vector<std::vector<glm::vec3>>(resolution, std::vector<glm::vec3>(resolution));
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            // The corner cases of [0,0] or [resolution - 1, resolution - 1]
            if ((i == 0 && j == 0)) {
                glm::vec3 right = mesh[i][j + 1] - mesh[i][j];
                glm::vec3 down = mesh[i + 1][j] - mesh[i][j];
                glm::vec3 normal = glm::cross(right, down);
                vertices_normals[i][j] = glm::normalize(normal);
            } else if (i == resolution - 1 && j == resolution -1){
                glm::vec3 left = mesh[i][j - 1] - mesh[i][j];
                glm::vec3 up = mesh[i - 1][j] - mesh[i][j];
                glm::vec3 normal = glm::cross(left, up);
                vertices_normals[i][j] = glm::normalize(normal);
            // The corner cases of [0, resolution - 1] or [resolution - 1, 0]
            } else if (i == 0 && j == resolution - 1){
                glm::vec3 left = mesh[i][j - 1] - mesh[i][j];
                glm::vec3 down = mesh[i + 1][j] - mesh[i][j];
                glm::vec3 downLeft = mesh[i + 1][j - 1] - mesh[i][j];
                glm::vec3 normal = glm::cross(left, down) + glm::cross(down, downLeft);
                vertices_normals[i][j] = glm::normalize(normal);
            } else if (i == resolution - 1 && j == 0){
                glm::vec3 right = mesh[i][j + 1] - mesh[i][j];
                glm::vec3 up = mesh[i - 1][j] - mesh[i][j];
                glm::vec3 upRight = mesh[i - 1][j + 1] - mesh[i][j];
                glm::vec3 normal = glm::cross(right, up) + glm::cross(up, upRight);
                vertices_normals[i][j] = glm::normalize(normal);
            // The top side
            } else if (i == 0){
                glm::vec3 left = mesh[i][j - 1] - mesh[i][j];
                glm::vec3 right = mesh[i][j + 1] - mesh[i][j];
                glm::vec3 down = mesh[i + 1][j] - mesh[i][j];
                glm::vec3 downLeft = mesh[i + 1][j - 1] - mesh[i][j];
                glm::vec3 normal = glm::cross(left, downLeft) + glm::cross(downLeft, down) + glm::cross(down, right);
                vertices_normals[i][j] = glm::normalize(normal);
            // The bottom side
            } else if (i == resolution - 1){
                glm::vec3 left = mesh[i][j - 1] - mesh[i][j];
                glm::vec3 right = mesh[i][j + 1] - mesh[i][j];
                glm::vec3 up = mesh[i - 1][j] - mesh[i][j];
                glm::vec3 upRight = mesh[i - 1][j + 1] - mesh[i][j];
                glm::vec3 normal = glm::cross(left, up) + glm::cross(up, upRight) + glm::cross(upRight, right);
                vertices_normals[i][j] = glm::normalize(normal);
            // The left side
            } else if (j == 0){
                glm::vec3 right = mesh[i][j + 1] - mesh[i][j];
                glm::vec3 up = mesh[i - 1][j] - mesh[i][j];
                glm::vec3 down = mesh[i + 1][j] - mesh[i][j];
                glm::vec3 upRight = mesh[i - 1][j + 1] - mesh[i][j];
                glm::vec3 normal = glm::cross(up, upRight) + glm::cross(upRight, right) + glm::cross(right, down);
                vertices_normals[i][j] = glm::normalize(normal);
            // The right side
            } else if (j == resolution - 1){
                glm::vec3 left = mesh[i][j - 1] - mesh[i][j];
                glm::vec3 up = mesh[i - 1][j] - mesh[i][j];
                glm::vec3 down = mesh[i + 1][j] - mesh[i][j];
                glm::vec3 downLeft = mesh[i + 1][j - 1] - mesh[i][j];
                glm::vec3 normal = glm::cross(down, downLeft) + glm::cross(downLeft, left) + glm::cross(left, up);
                vertices_normals[i][j] = glm::normalize(normal);
            // The general case for all other points
            } else {
                glm::vec3 up = mesh[i - 1][j] - mesh[i][j];
                glm::vec3 upRight = mesh[i - 1][j + 1] - mesh[i][j];
                glm::vec3 right = mesh[i][j + 1] - mesh[i][j];
                glm::vec3 down = mesh[i + 1][j] - mesh[i][j];
                glm::vec3 downLeft = mesh[i + 1][j - 1] - mesh[i][j];
                glm::vec3 left = mesh[i][j - 1] - mesh[i][j];
                glm::vec3 normal = glm::cross(up, upRight) + glm::cross(upRight, right) + glm::cross(right, down) + glm::cross(down, downLeft) + glm::cross(downLeft, left) + glm::cross(left, up);
                vertices_normals[i][j] = glm::normalize(normal);
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

// TODO: Implement a function to save the rendered mesh to an obj file
void storeToObj(std::vector<glm::vec3> vertices, std::vector<glm::vec3> indices, std::vector<glm::vec3> normals, int resolution, int indexBufferSize, std::string path) {
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
    for (int i = 0; i < indexBufferSize; i++) {
        objFile << "f " << indices[i][0] + 1 << "//" << indices[i][0] + 1 << " " << indices[i][1] + 1 << "//" << indices[i][1] + 1 << " " << indices[i][2] + 1 << "//" << indices[i][2] + 1 << std::endl;
    }
    objFile.close();
}

// TODO: Implement a function to load the obj and render it using OpenGL

int main(void) {
    // Read the environment variable $PROJECT_ROOT
    std::string project_root = std::getenv("PROJECT_ROOT");
    // Read the heightmap
    std::string filename = project_root + "/data/simplex_heightmap.raw";
    int heightmapSize = 512;
    double **heightmap = read_heightmap(filename.c_str(), heightmapSize);
    // Print the heightmap
    // for (int i = 0; i < size; i++) {
    //     for (int j = 0; j < size; j++) {
    //         std::cout << heightmap[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << "First few values: " << std::endl;
    // std::cout << heightmap[0][0] << std::endl;
    // std::cout << heightmap[0][1] << std::endl;
    // std::cout << heightmap[1][0] << std::endl;

    // Generate the mesh
    int meshResolution = 512;
    std::vector<std::vector<glm::vec3>> mesh = generate_mesh(heightmap, heightmapSize, meshResolution);
    // Print the first few values
    // std::cout << "First few values: " << std::endl;
    // std::cout << mesh[0][0][0] << " " << mesh[0][0][1] << " " << mesh[0][0][2] << std::endl;
    // std::cout << mesh[0][1][0] << " " << mesh[0][1][1] << " " << mesh[0][1][2] << std::endl;
    // std::cout << mesh[0][2][0] << " " << mesh[0][2][1] << " " << mesh[0][2][2] << std::endl;
    // std::cout << mesh[1][0][0] << " " << mesh[1][0][1] << " " << mesh[1][0][2] << std::endl;

    // Flatten the mesh
    std::vector<glm::vec3> flattened_mesh = flatten_mesh(mesh, meshResolution);
    // Print the first few values
    // std::cout << "First few values: " << std::endl;
    // std::cout << flattened_mesh[0][0] << " " << flattened_mesh[0][1] << " " << flattened_mesh[0][2] << std::endl;
    // std::cout << flattened_mesh[1][0] << " " << flattened_mesh[1][1] << " " << flattened_mesh[1][2] << std::endl;
    // std::cout << flattened_mesh[2][0] << " " << flattened_mesh[2][1] << " " << flattened_mesh[2][2] << std::endl;
    // std::cout << flattened_mesh[1024][0] << " " << flattened_mesh[1024][1] << " " << flattened_mesh[1024][2] << std::endl;

    std::vector<glm::vec3> index_buffer = generate_index_buffer(meshResolution);
    // Print the first few values
    // std::cout << "First few values: " << std::endl;
    // std::cout << index_buffer[0][0] << " " << index_buffer[0][1] << " " << index_buffer[0][2] << std::endl;
    // std::cout << index_buffer[1][0] << " " << index_buffer[1][1] << " " << index_buffer[1][2] << std::endl;
    // std::cout << index_buffer[2][0] << " " << index_buffer[2][1] << " " << index_buffer[2][2] << std::endl;
    // std::cout << index_buffer[3][0] << " " << index_buffer[3][1] << " " << index_buffer[3][2] << std::endl;

    std::vector<std::vector<glm::vec3>> vertices_normals = generate_vertices_normals(mesh, meshResolution);
    std::vector<glm::vec3> flattened_normals = flatten_normals(vertices_normals, meshResolution);

    int indexBufferSize = (meshResolution - 1) * (meshResolution - 1) * 2;
    std::string path = project_root + "/data/simplex_mesh_with_normals.obj";
    storeToObj(flattened_mesh, index_buffer, flattened_normals, meshResolution, indexBufferSize, path);
    return 0;
}