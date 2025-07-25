// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

// this vertex shader is taken and modified from lab 4.8 vertex shader

// according to https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL)
// using a layout modifier, assign attrib in program is no longer necessary
// intended to be handled by program, less likely needed to be modified
layout (location = 0) in vec3 VertexPosition;
// layout (location = 1) in vec3 VertexNormal;  // lab 9.1

// by default the system send corresponding data to specific port
// this one is for to receive vec2 VertexTexCoord, so as 0 and 1 did their jobs
layout (location = 2) in vec2 VertexTexCoord;   // for texture receiving

// send to fragment shader or other shader
// out vec3 VPosition;
// out vec3 VNormal;
out vec2 TexCoord;  // lab 9.1

// intended to be handled by user, do what you want, also they could be left empty
//uniform mat4 ModelViewMatrix;
//uniform mat3 NormalMatrix;
//uniform mat4 ProjectionMatrix;
uniform mat4 MVP;   // lab 9.1


void main()
{
	TexCoord = VertexTexCoord;
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}