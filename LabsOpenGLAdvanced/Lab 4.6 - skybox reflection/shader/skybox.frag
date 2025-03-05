#version 460

///
/// In fragment shader
/// for how many variables are displayed as in 
/// same variables should come out from vertex shader
///

// intended to be handled by program, less likely needed to be modified
in vec3 position;       // rename to LightDir in Lab 4 part 5 video

// lab 4.1 - single texture

in vec2 TexCoord;

// according to https://blog.mmorpgtycoon.com/post/opengl-texture-inconsistent/
// lets assume the binding word related to the "slot number" of a texture unit
// so without actually using the relative texture unit with the number binded here
// it gone wrong and causing glitchy texture
//
// also maybe the type have to be the same as well
//
// actual texture to be imported from program
// layout (binding = 0) uniform sampler2D Tex1;    // in the lab it is renamed
layout (binding = 0) uniform samplerCube Tex1;    // in the lab it is renamed

// lab 4.1

// Final Output
layout (location = 0) out vec4 FragColor;

// Phong model not necessary for lab 4.5

void main()
{
    // lab 4.5

    // vec3 texColor = texture(skyBoxTex, normalize(Vec)).rgb;
    vec3 texColor = texture(Tex1, normalize(position)).rgb;

    texColor = pow(texColor, vec3(1.0 / 2.2));

    // Basic output logic
    // FragColor = vec4(phongModel(position, normalize(normal)), 1.0);
    FragColor = vec4(texColor, 1.0);
    
    // lab 4.5
}
