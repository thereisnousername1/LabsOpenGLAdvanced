// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// send to fragment shader or other shader
out vec3 normal;
out vec3 position;

out vec4 ShadowCoord;       // lab 8.1

// intended to be handled by user, do what you want, also they could be left empty
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

uniform mat4 ShadowMatrix;  // lab 8.1

void getCamSpaceValues(out vec3 n, out vec3 pos)
{
    // convert the vertex normal to eye coordinates
    n = normalize(NormalMatrix * VertexNormal);

    // convert the vertex position to eye coordinates
    pos = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
}

void main()
{
    // lab 8.1

    ShadowCoord = ShadowMatrix * vec4(VertexPosition, 1.0);

    // lab 8.1

    getCamSpaceValues(normal, position);

    gl_Position = MVP * vec4(VertexPosition,1.0);

}