// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

// under the setting of using a Phong lighting, main mechanic should take place in fragment shader

// according to https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL)
// using a layout modifier, assign attrib in program is no longer necessary
// intended to be handled by program, less likely needed to be modified
layout (location = 0) in vec3 VertexPosition;

out vec3 position;

// complex logic is not necessary in lab 4.5

///
/// Starting from the lab 3 Ld and Ls is combined into 1 which means whether the light intensity is getting strong or weak,
/// both diffuse and specular setting are doing the same. It is reasonable but I want to keep them separate.
/// That way is more interesting.
///

// intended to be handled by user, do what you want, also they could be left empty
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

uniform mat4 ProjectionMatrix;  // unknown, added from lab 3.1 and never use

void main()
{
    // In lab it is Vec = VertexPosition;
    position = VertexPosition;
    gl_Position = MVP * vec4(VertexPosition,1.0);

}