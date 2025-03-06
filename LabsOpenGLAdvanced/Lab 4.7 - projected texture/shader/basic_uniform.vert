// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

// under the setting of using a Phong lighting, main mechanic should take place in fragment shader

// according to https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL)
// using a layout modifier, assign attrib in program is no longer necessary
// intended to be handled by program, less likely needed to be modified
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// lab 4.7

// by default the system send corresponding data to specific port
// this one is for to receive vec2 VertexTexCoord, so as 0 and 1 did their jobs
// layout (location = 2) in vec2 VertexTexCoord;   // disabled since lab 4.7

// out vec2 TexCoord;  // renamed to ProjTexCoord since lab 4.7
out vec4 TexCoord;

// send to fragment shader or other shader
out vec3 normal;    // renamed to EyeNormal since lab 4.7
out vec3 position;  // renamed to EyePosition since lab 4.7

///
/// Starting from the lab 3 Ld and Ls is combined into 1 which means whether the light intensity is getting strong or weak,
/// both diffuse and specular setting are doing the same. It is reasonable but I want to keep them separate.
/// That way is more interesting.
///

// intended to be handled by user, do what you want, also they could be left empty
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

uniform mat4 ProjectionMatrix;  // finally ProjectionMatrix is being use in lab 4.7
                                // somehow named ModelMatrix in tutorial video lab 4 part 7

uniform mat4 ProjectorMatrix;

// lab 4.7

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
    // lab 4.7

    // EyeNormal = normalize(NormalMatrix * VertexNormal);
    // EyePosition = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
    getCamSpaceValues(normal, position);    // similar logic on top are used in tutorial video 
    
    //TexCoord = VertexTexCoord;

    //// Approximately (I guess), read https://gamedev.stackexchange.com/questions/136007/projection-texture-mapping may help
    // ~actual mapping position = 
    //                ~where you want the obj to be project locate in scene 
    //                                * (~local transform of the model itself * vec4(VertexPosition, 1.0)
    // ProjTexCoord = ProjectorMatrix * (ModelMatrix * vec4(VertexPosition, 1.0));  // lab solution
    TexCoord = ProjectorMatrix * (ProjectionMatrix * vec4(VertexPosition, 1.0));

    gl_Position = MVP * vec4(VertexPosition,1.0);

    // lab 4.7

}