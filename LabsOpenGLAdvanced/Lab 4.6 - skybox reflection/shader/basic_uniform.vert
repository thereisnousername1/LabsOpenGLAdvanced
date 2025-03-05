// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

// this shader is taken from lab 4.1, modified for lab 4.6

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

//out vec2 TexCoord;

//out vec3 normal;
//out vec3 position;
out vec3 ReflectDir;    // reflecting like chromium surface

///
/// Starting from the lab 3 Ld and Ls is combined into 1 which means whether the light intensity is getting strong or weak,
/// both diffuse and specular setting are doing the same. It is reasonable but I want to keep them separate.
/// That way is more interesting.
///

// intended to be handled by user, do what you want, also they could be left empty
// uniform mat4 ModelViewMatrix;   // not set in lab 4.6
uniform mat4 MVP;
// uniform mat3 NormalMatrix;      // not set in lab 4.6

uniform mat4 ProjectionMatrix;  // unknown, added from lab 3.1 and never use

// lab 4.6

uniform mat4 ModelMatrix;

uniform vec3 WorldCameraPosition;

// lab 4.6

void main()
{

    // no getting Cam space value in lab 4.6
    //getCamSpaceValues(normal, position);

    // lab 4.6

    vec3 worldPos = vec3(ModelMatrix * vec4(VertexPosition, 1.0));
    vec3 worldNorm = vec3(ModelMatrix * vec4(VertexNormal, 0.0));
    vec3 worldView = normalize(WorldCameraPosition - worldPos);

    // ReflectDir = -reflect(-worldView, worldNorm);    // this revert the whole reflection image
    ReflectDir = reflect(-worldView, worldNorm);

    // lab 4.6

    gl_Position = MVP * vec4(VertexPosition,1.0);

}