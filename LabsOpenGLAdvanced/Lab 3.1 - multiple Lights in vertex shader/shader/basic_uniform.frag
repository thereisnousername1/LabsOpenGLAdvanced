#version 460

///
/// In fragment shader
/// for how many variables are displayed as in 
/// same variables should come out from vertex shader
///

// lab 2

// lab 2 - flat shading
// in vec3 LightIntensity;
// flat in vec3 LightIntensity; // disabled since lab 3.1

// lab 2

in vec3 Color;
layout (location = 0) out vec4 FragColor;


void main() {

    // lab 3.1

    FragColor = vec4(Color, 1.0);
    // FragColor = vec4(LightIntensity, 1.0);

    // lab 3.1
}
