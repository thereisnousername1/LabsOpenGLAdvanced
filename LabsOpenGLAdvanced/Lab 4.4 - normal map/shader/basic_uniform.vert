// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

// under the setting of using a Phong lighting, main mechanic should take place in fragment shader

// according to https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL)
// using a layout modifier, assign attrib in program is no longer necessary
// intended to be handled by program, less likely needed to be modified
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// lab 4.1 - single texture

// by default the system send corresponding data to specific port
// this one is for to receive vec2 VertexTexCoord, so as 0 and 1 did their jobs
layout (location = 2) in vec2 VertexTexCoord;   // for texture receiving

out vec2 TexCoord;

// lab 4.1

// lab 4.4 - normal map

layout (location = 3) in vec4 VertexTangent;

uniform struct LightInfo
{
    vec4 Position;  // could be static or moving, depends on logic in the program

    vec3 Ld;    // diffuse light intensity
    vec3 La;    // ambient light intensity
    vec3 Ls;    // specular light intensity

}Light;

// lab 4.4 - normal map

// send to fragment shader or other shader
out vec3 normal;        // rename to ViewDir in Lab 4 part 5 video
out vec3 position;      // rename to LightDir in Lab 4 part 5 video

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
    // lab 4.4 - normal map - reference to OpenGL SuperBible p.582(Ch. 13)

    // rough decision model -> comply with fragment shader
    // decide using normal map or program generate for user
    // refer to COMP3016 CW2 switch case

    // geez get a better name
    // calculate normal and tangent vectors in view space from
    // incoming object space vectors - OpenGL SuperBible p.584
    vec3 norm = normalize(NormalMatrix * VertexNormal);
    vec3 tangent = normalize(NormalMatrix * vec3(VertexTangent));

    // calculate the bitangent vector from the normal and
    // tangent vectors - OpenGL SuperBible p.584
    vec3 binormal = normalize(cross(norm, tangent)) * VertexTangent.w;

    // this I don't know
    mat3 toObjectLocal = mat3(
        tangent.x, binormal.x, norm.x,
        tangent.y, binormal.y, norm.y,
        tangent.z, binormal.z, norm.z
    );
    
    // getCamSpaceValues(normal, position); // replaced since lab 4.4

    // convert the vertex position to eye coordinates
    vec3 Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;

    // to calculate this both shader needs the Light struct
    // LightDir = toObjectLocal * (Light.Position.xyz - position);   // from video
    position = toObjectLocal * (Light.Position.xyz - Position);

    // ViewDir = toObjectLocal * normalize(-position);  // from video
    normal = toObjectLocal * normalize(-Position);

    // lab 4.4

    // lab 4.1

    TexCoord = VertexTexCoord;

    // lab 4.1

    gl_Position = MVP * vec4(VertexPosition,1.0);

}