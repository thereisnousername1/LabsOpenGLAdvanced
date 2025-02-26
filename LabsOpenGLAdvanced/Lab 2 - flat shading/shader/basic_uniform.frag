#version 460

// lab 2

// lab 2 - flat shading
// in vec3 LightIntensity;
flat in vec3 LightIntensity;

// lab 2

in vec3 Color;
layout (location = 0) out vec4 FragColor;


void main() {

    // lab 2

    // FragColor = vec4(Color, 1.0);
    FragColor = vec4(LightIntensity, 1.0);

    // lab 2
}
