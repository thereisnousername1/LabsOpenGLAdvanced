// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

///
/// lab 3 comes from lab 2.3
/// lines and comments could be deleted for better readability
/// especially the long abandoned one
///

layout (location = 0) in vec3 VertexPosition;

layout (location = 1) in vec3 VertexNormal;

// out vec3 Color; // Color is handled in fragment shader this time

// lab 3.2

out vec3 normal;

out vec3 position;

// lab 3.2

// lab 2 - flat shading
// out vec3 LightIntensity; // disabled since lab 2 - flat shading
// flat out vec3 LightIntensity;    // disabled since lab 3.1
// also the fragment shader

// lab 2

///
/// Starting from the lab 3 Ld and Ls is combined into 1 which means whether the light intensity is getting strong or weak,
/// both diffuse and specular setting are doing the same. It is reasonable but I want to keep them separate.
/// That way is more interesting.
///

// lab 3.2

// As LightInformation class and MaterialInformation class are both necessary information for phong model
// they are moved to fragment shader as well

// lab 3.2

uniform mat4 ModelViewMatrix;

uniform mat4 MVP;

uniform mat3 NormalMatrix;

// lab 3.1

uniform mat4 ProjectionMatrix;

// lab 3.1

// this function is called in main()
void getCamSpaceValues(out vec3 n, out vec3 pos)
{
    // convert the vertex normal to eye coordinates
    n = normalize(NormalMatrix * VertexNormal);

    // convert the vertex position to eye coordinates
    pos = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
}

// lab 3.1 - modified from final code in lab 2.3, modified for readability

// vec3 phongModel() moved to fragment shader, so as the related code

// lab 3.1

void main()
{
    /* lab 2 - diffuse - separated from main() in lab 3.1

    ...
    there used to be lines, deleted for readability

    */

    // normalized normal
    // page 5 of lecture 2 notes(COMP3015)
    // convert the vertex normal to eye coordinates
    // vec3 n, pos;

    // for a void function constructed in the shader
    // it seems the variables with the out modifier cannot be the same name as 
    // the actual global variables to be out to the next stage (in here it is fragment shader)
    // so i created 2 new global variables to be out to the next stage
    // and replaced them in the line
    //
    // also don't forget to add in variables to receive the 2 newly created output
    // getCamSpaceValues(n, pos);
    getCamSpaceValues(normal, position);

    /* lab 3.2 - moved to fragment shader

    Color = vec3(0.0);  // init Color

    //  total number of lights
    for (int i = 0; i < 3; i++)
    {
        // final output of each light += phongModel(index of lights, camCoords in the scene, normalized normal vertex)
        Color += phongModel(i, pos, n);
        // go to scenebasic_uniform class and fragment shader to change the details
    }

    */

    gl_Position = MVP * vec4(VertexPosition,1.0);

}
