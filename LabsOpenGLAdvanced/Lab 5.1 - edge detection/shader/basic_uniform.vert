// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

// under the setting of using a Phong lighting, main mechanic should take place in fragment shader

// according to https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL)
// using a layout modifier, assign attrib in program is no longer necessary
// intended to be handled by program, less likely needed to be modified
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// since lab 5.1 single texture is not replaced

// send to fragment shader or other shader
out vec3 normal;
out vec3 position;

///
/// Starting from the lab 3 Ld and Ls is combined into 1 which means whether the light intensity is getting strong or weak,
/// both diffuse and specular setting are doing the same. It is reasonable but I want to keep them separate.
/// That way is more interesting.
///

// intended to be handled by user, do what you want, also they could be left empty
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

// this function is called in main()
//                     return value stored in the imported variables
void getCamSpaceValues(out vec3 n, out vec3 pos)
{
    // convert the vertex normal to eye coordinates
    n = normalize(NormalMatrix * VertexNormal);

    // convert the vertex position to eye coordinates
    pos = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
}

void main()
{
    getCamSpaceValues(normal, position);

    gl_Position = MVP * vec4(VertexPosition,1.0);

}