#version 330 core
out vec4 FragColor;

in vec3 worldPos;

// uniform vec3 colour;

void main()
{

    float maxHeight = 192.0;
    float minHeight = 0.0;
    // We are going to colour the fragment based on the height of the fragment
    // Black at the bottom and white at the top
    float height = worldPos.y;
    float red = (height - minHeight) / (maxHeight - minHeight);
    FragColor = vec4(red, 0.0, 0.0, 1.0);

}