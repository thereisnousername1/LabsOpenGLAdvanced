// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

layout (location = 0) in vec3 VertexPosition;

// lab 2

// layout (location = 1) in vec3 VertexColor;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

// lab 2

// out vec3 Color; // useless since lab 2

// lab 1

// uniform mat4 RotationMatrix; // useless since lab 2

// lab 1

// lab 2 - diffuse

uniform vec4 LightPosition;

uniform vec3 Kd;

uniform vec3 Ld;

uniform mat4 ModelViewMatrix;

uniform mat4 MVP;

uniform mat3 NormalMatrix;

// lab 2 - diffuse

void main()
{
    // lab 2 - diffuse

    // Color = VertexColor;

    // normalized normal
    // page 5 of lecture 2 notes(COMP3015)
    // convert the vertex normal to eye coordinates
    vec3 n = normalize(NormalMatrix * VertexNormal);

    // convert the vertex position to eye coordinates
    vec4 pos = ModelViewMatrix * vec4(VertexPosition, 1.0);

    // light direction
    // calculate the direction from the light position to the vertex point with normalize
    vec3 s = normalize(vec3(LightPosition - pos));

    // According to OpenGL SuperBible p.569 (CH13 - Diffuse Light), this dot product caould be a negative value
    // max() is placed to prevent such situation, it will justify the negative part to zero
    // calculate the dot product of normalized normal and light direction
    float sDotN = max(dot(s, n), 0.0);

    // experiment when we don't use max()
    // float sDotN = dot(s, n);

    // diffuseColor = diffuseMaterial * DiffuseLight * the dot product from normal and light direction
    vec3 diffuse = Ld * Kd * sDotN;

    // final ouput = diffuse, in the following lab there could be more different light implemented and output with diffuse
    LightIntensity = diffuse;

    // lab 1
    // gl_Position = vec4(VertexPosition,1.0);  // disabled from lab 1
    // gl_Position = vec4(VertexPosition,1.0) * RotationMatrix; // diabled from lab 2
    gl_Position = MVP * vec4(VertexPosition,1.0);

    // lab 2 - diffuse
}
