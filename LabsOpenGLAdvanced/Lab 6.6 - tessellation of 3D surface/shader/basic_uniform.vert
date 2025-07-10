// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

// since lab 6 geometry shader is introduced and the structure of vertex shader is modified

layout (location = 0) in vec3 VertexPosition;	// lab 6.6

/* not needed in lab 6.4

layout (location = 1) in vec3 VertexNormal;

out vec3 VNormal;	// Normal sent from vertex shader(to geometry shader)

out vec3 VPosition;	// Position sent from vertex shader(to geometry shader)

uniform mat4 ModelViewMatrix;

uniform mat3 NormalMatrix;

uniform mat4 MVP;

void main()
{
	// gl_Position = ModelViewMatrix * vec4(VertexPosition, 1.0);
	
	// Transform 'vertexNormal' into camera coordinates
	VNormal = normalize( NormalMatrix * VertexNormal );

	// Transform 'vertexPosition' into camera coordinates
	VPosition = vec3( ModelViewMatrix * vec4(VertexPosition, 1.0) );

	gl_Position = MVP * vec4(VertexPosition, 1.0);
}

*/ // lab 6.4

void main()
{
	gl_Position = vec4(VertexPosition, 1.0);
}