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

// lab 3.1

out vec3 Color; // Enabled again to replace LightIntensity

// lab 3.1

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

// lab 2 - phong model

uniform struct LightInfo        // extend to a class
{
    vec4 Position;

    vec3 Ld;    // diffuse light intensity

    vec3 La;    // ambient light intensity

    vec3 Ls;    // specular light intensity

// lab 3.1

// }Light; // used in lab 2, useless in lab 3
}lights[3]; // lab 3.1, 3 lights in the scene

// lab 3.1

uniform struct MaterialInfo        // extend to a class
{
    vec3 Kd;    // diffuse Material reflectivity

    vec3 Ka;    // ambient Material reflectivity

    vec3 Ks;    // specular Material reflectivity

    // how shiny the specular light is
    float shininess;

}Material;

// lab 2 - phong model

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

vec3 phongModel (int light, vec3 position, vec3 n)
{

//// ambient lighting is a constant & omnipresent light source
    // vec3 ambient = Light.La * Material.Ka;
    vec3 ambient = lights[light].La * Material.Ka;

    // lab 2 - phong model

    // light direction
    // calculate the direction from the light position to the vertex point with normalize
    // vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 s = normalize(vec3(lights[light].Position) - position);    // find out s vector

    // According to OpenGL SuperBible p.569 (CH13 - Diffuse Light), this dot product caould be a negative value
    // max() is placed to prevent such situation, it will justify the negative part to zero
    float sDotN = max(dot(s, n), 0.0);    // calculate the dot product of normalized normal and light direction

    // experiment when we don't use max()
    // float sDotN = dot(s, n);

//// diffuse lighting is directional lighting
    // diffuseColor = diffuseMaterial * DiffuseLight * the dot product from normal and light direction
    // vec3 diffuse = Light.Ld * Material.Kd * sDotN;
    vec3 diffuse = lights[light].Ld * Material.Kd * sDotN;

    //// In the lab pdf, diffuse intensity and specular intensity is combined
    //// they are meant to be multiply in the final output
    //// so originally diffuse in here will only combined Material.Kd and sDotN together

//// specular lighting is meant to represent glare upon an object's surface
    vec3 specular = vec3(0.0);

    if (sDotN > 0.0)
    {
        // p.569 of opengl superbible
        // eye normal
        // vec3 v = normalize(-pos.xyz);
        vec3 v = normalize(-position.xyz);

        // reflection
        vec3 r = reflect(-s, n);

        //  final spec                           eye reflection angle
        // specular = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);
        specular = lights[light].Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);
    
        //// In the lab pdf, diffuse intensity and specular intensity is combined
        //// they are meant to be multiply in the final output
        //// so originally specular in here will only combined Material.Ks and
        //// pow(max(dot(r, v), 0.0), Material.shininess) together

    }

    // final ouput
    // LightIntensity = diffuse + ambient + specular;
    return diffuse + ambient + specular;   // Color (LightIntensity is disabled) is the final output in of the whole product
                                           // therefore it should exists only in the main()
                                           // a better programming structure manner

    //// Originally lights[light].L are meant to be merge here
    //// the code looks like this
    //// return ambient + lights[light].L * (diffuse + specular);
    //// for the lights[light].L only apply to diffuse and specular
    //// while ambient remains separated, cuz it is unique in the scene
}

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
    vec3 n, pos;

    getCamSpaceValues(n, pos);

    // lab 3.1

    Color = vec3(0.0);  // init Color

    //  total number of lights
    for (int i = 0; i < 3; i++)
    {
        // final output of each light += phongModel(index of lights, camCoords in the scene, normalized normal vertex)
        Color += phongModel(i, pos, n);
        // go to scenebasic_uniform class and fragment shader to change the details
    }

    // lab 3.1

    gl_Position = MVP * vec4(VertexPosition,1.0);

}
