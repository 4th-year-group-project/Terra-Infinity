#include <iostream>
#include <fstream>
#include <cmath>

#include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
#include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#include "/dcs/large/efogahlewem/.local/include/opencv4/opencv2/opencv.hpp"

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
        return value * scale_factor;
    case 1:
        // This is the power smoothing function
        return sgn(value) * pow(abs(value), function_factor) * scale_factor;
    case 2:
        // This is sigmoid smoothing function using tanh
        return scale_factor * (tanh(function_factor * value) / tanh(function_factor));
    case 3:
        // This is logarithmic smoothing function
        return sgn(value) * (log(1 + function_factor * ((value + 1)/ 2))/ log(1+ function_factor));
    case 4:
        // This is a hybrid approach between linear and sigmoid smoothing
        return (1-blend_factor) * value * scale_factor + blend_factor * (scale_factor * (tanh(function_factor * value) / tanh(function_factor)));
    default:
        // There is no scaling applied
        return value;
    }
}

// A function to generate a mesh from a heightmap using bilinear interpolation where necessary if the resolution of the
// mesh is higher than the resolution of the heightmap
// Size is the size of the heightmap and resolution is the resolution of the mesh
double ***generate_mesh(double **heightmap, int size, int resolution) {
    // We want to scale the heightmap values from the range [-1, 1] to the range [-256, 256]
    double function_factor = 5.0;
    double scaling_factor = 128.0;
    // We want to create a mesh with resolution x resolution vertices, each vertex will have three coordinates
    double ***mesh = new double**[resolution];
    for (int i = 0; i < resolution; i++) {
        mesh[i] = new double*[resolution];
        for (int j = 0; j < resolution; j++) {
            mesh[i][j] = new double[3];
        }
    }
    // Compute the step size for the mesh based on the size of the heightmap and the resolution of the mesh
    double step = static_cast<double>(size) / static_cast<double>(resolution);
    std::cout << "Step: " << step << std::endl;
    // Iterate over the mesh and compute the height of each vertex
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            // Compute the coordinates of the vertex in the heightmap
            double x = j * step;
            double z = i * step;
            // std::cout << "X: "<< x << " Y: " << y << std::endl;
            // Compute the coordinates of the four vertices in the heightmap that surround the vertex
            double x1 = static_cast<int>(x);
            double x2 = static_cast<int>(x) + 1;
            double z1 = static_cast<int>(z);
            double z2 = static_cast<int>(z) + 1;
            // std::cout << "X1: " << x1 << " X2: " << x2 << " Y1: " << y1 << " Y2: " << y2 << std::endl;
            // Check if the vertex is on the edge of the heightmap
            if ((x2 >= size || z2 >= size) || (x1 == x && z1 == z && x2 == x + 1 && z2 == z + 1)) {
                //  If the vertex is on the edge of the heightmap, we can't use bilinear interpolation
                //  We will just use the height of the closest vertex
                //  Or the vertex is a whole number for all coordinates then we dont need to interpolate
                mesh[i][j][0] = x;
                mesh[i][j][1] = height_scaling(heightmap[static_cast<int>(z)][static_cast<int>(x)], 0, scaling_factor, function_factor);
                mesh[i][j][2] = z;
            // Check if the vertex is a whole number for all coordinates
            } else {
                // If the vertex is not on the edge of the heightmap, we can use bilinear interpolation
                mesh[i][j][0] = x;
                double bilinear_interpolation_value = bilinear_interpolation(x, z, heightmap, x1, x2, z1, z2);
                mesh[i][j][1] = height_scaling(bilinear_interpolation_value, 0, scaling_factor, function_factor);
                mesh[i][j][2] = z;
            }
        }
    }
    return mesh;
}

// This produced our flattened mesh which can be the vertex buffer for OpenGL
double **flatten_mesh(double ***mesh, int resolution) {
    double **flattened_mesh = new double*[resolution * resolution];
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            flattened_mesh[i * resolution + j] = new double[3];
            flattened_mesh[i * resolution + j][0] = mesh[i][j][0];
            flattened_mesh[i * resolution + j][1] = mesh[i][j][1];
            flattened_mesh[i * resolution + j][2] = mesh[i][j][2];
        }
    }
    return flattened_mesh;
}

// We need to generate the index buffer for OpenGL to render the mesh
// Each quad will have 2 triangles and each triangle will have 3 vertices
// The widing order of the triangles is important and is clockwise
int **generate_index_buffer(int resolution) {
    int **index_buffer = new int*[(resolution - 1) * (resolution - 1) * 2];
    for (int i = 0; i < resolution - 1; i++) {
        for (int j = 0; j < resolution - 1; j++) {
            // First triangle
            index_buffer[i * (resolution - 1) * 2 + j * 2] = new int[3];
            index_buffer[i * (resolution - 1) * 2 + j * 2][0] = i * resolution + j;
            index_buffer[i * (resolution - 1) * 2 + j * 2][1] = i * resolution + j + 1;
            index_buffer[i * (resolution - 1) * 2 + j * 2][2] = (i + 1) * resolution + j;
            // Second triangle
            index_buffer[i * (resolution - 1) * 2 + j * 2 + 1] = new int[3];
            index_buffer[i * (resolution - 1) * 2 + j * 2 + 1][0] = i * resolution + j + 1;
            index_buffer[i * (resolution - 1) * 2 + j * 2 + 1][1] = (i + 1) * resolution + j + 1;
            index_buffer[i * (resolution - 1) * 2 + j * 2 + 1][2] = (i + 1) * resolution + j;
        }
    }
    return index_buffer;
}





// TODO: Implement a function to save the rendered mesh to an obj file
void storeToObj(double **vertices, int **indices, int resolution, int indexBufferSize) {
    std::ofstream objFile;
    objFile.open("output.obj");
    objFile << "# OBJ file" << std::endl;
    objFile << "# Vertices" << std::endl;
    for (int i = 0; i < resolution * resolution; i++) {
        objFile << "v " << vertices[i][0] << " " << vertices[i][1] << " " << vertices[i][2] << std::endl;
    }
    objFile << "# Faces" << std::endl;
    for (int i = 0; i < indexBufferSize; i++) {
        objFile << "f " << indices[i][0] + 1 << " " << indices[i][1] + 1 << " " << indices[i][2] + 1 << std::endl;
    }
    objFile.close();
}

// TODO: Implement a function to load the obj and render it using OpenGL

int main(void) {
    // Read the heightmap
    std::string filename = "/dcs/21/u2102661/Documents/Group_Project/World-Generation/heightmap.raw";
    int size = 512;
    double **heightmap = read_heightmap(filename.c_str(), size);
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
    int resolution = 512;
    double ***mesh = generate_mesh(heightmap, size, resolution);
    // Print the first few values
    // std::cout << "First few values: " << std::endl;
    // std::cout << mesh[0][0][0] << " " << mesh[0][0][1] << " " << mesh[0][0][2] << std::endl;
    // std::cout << mesh[0][1][0] << " " << mesh[0][1][1] << " " << mesh[0][1][2] << std::endl;
    // std::cout << mesh[0][2][0] << " " << mesh[0][2][1] << " " << mesh[0][2][2] << std::endl;
    // std::cout << mesh[1][0][0] << " " << mesh[1][0][1] << " " << mesh[1][0][2] << std::endl;

    // Flatten the mesh
    double **flattened_mesh = flatten_mesh(mesh, resolution);
    // Print the first few values
    // std::cout << "First few values: " << std::endl;
    // std::cout << flattened_mesh[0][0] << " " << flattened_mesh[0][1] << " " << flattened_mesh[0][2] << std::endl;
    // std::cout << flattened_mesh[1][0] << " " << flattened_mesh[1][1] << " " << flattened_mesh[1][2] << std::endl;
    // std::cout << flattened_mesh[2][0] << " " << flattened_mesh[2][1] << " " << flattened_mesh[2][2] << std::endl;
    // std::cout << flattened_mesh[1024][0] << " " << flattened_mesh[1024][1] << " " << flattened_mesh[1024][2] << std::endl;

    int **index_buffer = generate_index_buffer(resolution);
    // Print the first few values
    // std::cout << "First few values: " << std::endl;
    // std::cout << index_buffer[0][0] << " " << index_buffer[0][1] << " " << index_buffer[0][2] << std::endl;
    // std::cout << index_buffer[1][0] << " " << index_buffer[1][1] << " " << index_buffer[1][2] << std::endl;
    // std::cout << index_buffer[2][0] << " " << index_buffer[2][1] << " " << index_buffer[2][2] << std::endl;
    // std::cout << index_buffer[3][0] << " " << index_buffer[3][1] << " " << index_buffer[3][2] << std::endl;

    int indexBufferSize = (resolution - 1) * (resolution - 1) * 2;
    storeToObj(flattened_mesh, index_buffer, resolution, indexBufferSize);
    return 0;
}