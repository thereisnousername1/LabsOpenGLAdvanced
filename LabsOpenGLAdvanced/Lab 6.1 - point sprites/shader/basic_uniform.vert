// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

// since lab 6 geometry shader is introduced and the structure of vertex shader is modified

layout (location = 0) in vec3 VertexPosition;

out vec3 Position;

uniform mat4 ModelViewMatrix;

void main()
{
	gl_Position = ModelViewMatrix * vec4(VertexPosition, 1.0);
}