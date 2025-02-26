// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexColor;

out vec3 Color;

// lab 1
uniform mat4 RotationMatrix;

void main()
{
    Color = VertexColor;

    // lab 1
    // gl_Position = vec4(VertexPosition,1.0);
    gl_Position = vec4(VertexPosition,1.0) * RotationMatrix;
}
