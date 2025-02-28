#version 460

///
/// In fragment shader
/// for how many variables are displayed as in 
/// same variables should come out from vertex shader
///

// lab 2

// lab 2 - flat shading
// in vec3 LightIntensity;
// flat in vec3 LightIntensity; // disabled since lab 3.1

// lab 2

// lab 3.2

in vec3 normal;

in vec3 position;

// lab 3.2

//in vec3 Color;  // no need to receive Color, for it is process in here (fragment shader) now

layout (location = 0) out vec4 FragColor;



/////////////////// lab 3.2 - the whole phong shading model and necessary information ///////////////////

uniform struct LightInfo
{
    vec4 Position;

    vec3 Ld;    // diffuse light intensity

    vec3 La;    // ambient light intensity

    vec3 Ls;    // specular light intensity

}Light; // enabled since lab 3.6
// }lights[3]; // lab 3.1 multiple lights logic

/*  Replaced with LightInfo in lab 3.6

uniform struct SpotLightInfo
{
    // vec4 Position;  // changed to vec3 in lab 3.4
    vec3 Position;

    vec3 Ld;    // diffuse light intensity

    vec3 La;    // ambient light intensity

    vec3 Ls;    // specular light intensity

    vec3 Direction;

    float Exponent;

    float Cutoff;

}Spot;

*/

uniform struct MaterialInfo         // extend to a class
{
    vec3 Kd;    // diffuse Material reflectivity

    vec3 Ka;    // ambient Material reflectivity

    vec3 Ks;    // specular Material reflectivity

    // how shiny the specular light is
    float shininess;

}Material;

// lab 3.6 Fog uniform struct

uniform struct FogInfo
{
    float MaxDist;

    float MinDist;

    vec3 Color;

}Fog;

// lab 3.6 Fog uniform struct

// Toon Shading is removed since lab 3.6

// in the video Blinn-Phong is used, but well in my study I want to keep both
// this function is taken from my lab 3.2 code, with modified for lab 3.4 - spotlight
// vec3 phongModel (int light, vec3 position, vec3 n)                                   // disabled since lab 3.4
// vec3 phongSpotModel (vec3 position, vec3 n)                                          // since lab 3.6 spotlight logic disabled
vec3 phongModel (vec3 position, vec3 n)
{

//// diffuse lighting is directional lighting
    vec3 diffuse = vec3(0.0);

//// specular lighting is meant to represent glare upon an object's surface
    vec3 specular = vec3(0.0);

//// ambient lighting is a constant & omnipresent light source
    vec3 ambient = Light.La * Material.Ka;  // enabled since lab 3.6
    // vec3 ambient = Spot.La * Material.Ka;    // disabled since lab 3.6

    // light direction
    // calculate the direction from the light position to the vertex point with normalize
    // vec3 s = normalize(vec3(Light.Position) - pos);
    // vec3 s = normalize(Spot.Position - position);    // disabled since lab 3.6
    vec3 s = normalize(Light.Position.xyz - position);    // find out s vector

    // Spotlight logic removed since lab 3.6

    // Toon Shading criteria checking removed since lab 3.6

    // According to OpenGL SuperBible p.569 (CH13 - Diffuse Light), this dot product caould be a negative value
    // max() is placed to prevent such situation, it will justify the negative part to zero
    float sDotN = max(dot(s, n), 0.0);    // calculate the dot product of normalized normal and light direction

    // experiment when we don't use max()
    // float sDotN = dot(s, n);

    // diffuseColor = diffuseMaterial * DiffuseLight * the dot product from normal and light direction
    diffuse = Light.Ld * Material.Kd * sDotN;   // enabled since lab 3.6, init is done on top
    // vec3 diffuse = Spot.Ld * Material.Kd * sDotN;

    //// In the lab pdf, diffuse intensity and specular intensity is combined
    //// they are meant to be multiply in the final output
    //// so originally diffuse in here will only combined Material.Kd and sDotN together

    if (sDotN > 0.0)
    {
        // p.569 of opengl superbible
        // eye normal
        vec3 v = normalize(-position.xyz);

        // reflection -> Phong Shading
        vec3 r = reflect(-s, n);

        // calculate the half vector -> Blinn-Phong Shading
        // vec3 h = normalize(s + v);

        //// For Spotlight
        // final spec                            eye reflection angle
        // specular = Spot.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);   // reflection -> Phong Shading
        // specular = Spot.Ls * Material.Ks * pow(max(dot(h, n), 0.0), Material.shininess);   // calculate the half vector -> Blinn-Phong Shading
    
        //// For Directional Light, use lights[light].Ls in multiple lights scenario
        // final spec                            eye reflection angle
        specular = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);   // reflection -> Phong Shading
        // specular = Light.Ls * Material.Ks * pow(max(dot(h, n), 0.0), Material.shininess);   // calculate the half vector -> Blinn-Phong Shading
    
        //// In the lab pdf, diffuse intensity and specular intensity is combined
        //// they are meant to be multiply in the final output
        //// so originally specular in here will only combined Material.Ks and
        //// pow(max(dot(r, v), 0.0), Material.shininess) together

    }

    // final ouput
    // LightIntensity = diffuse + ambient + specular;
    return diffuse + ambient + specular;   // to be called in main() (lab 3.6)
                                           // no need to be put inside any variable for it is single in the scene

    //// Originally lights[light].L are meant to be merge here
    //// the code looks like this
    //// return ambient + lights[light].L * (diffuse + specular);
    //// for the lights[light].L only apply to diffuse and specular
    //// while ambient remains separated, cuz it is unique in the scene
}

/////////////////// lab 3.2 - the whole phong shading model and necessary information ///////////////////

void main() {

    /* multiple lights are disabled

    vec3 Color = vec3(0.0); // for per-fragment lighting (phong shading), color is handled in fragment shader

    //  total number of lights
    for (int i = 0; i < 3; i++)
    {
        // final output of each light += phongModel(index of lights, camCoords in the scene, normalized normal vertex)
        // Color += phongModel(i, pos, n);
        // Color += phongModel(i, position, normal);    // disabled since lab 3.4
        Color += phongSpotModel(position, normalize(normal));
        // go to scenebasic_uniform class and fragment shader to change the details
    }

    FragColor = vec4(Color, 1.0);
    // FragColor = vec4(LightIntensity, 1.0);

    */

    // lab 3.6 Fog calculation logic

    // Calculate how the distance to the camera effects the level of fog (I guess?)
    float dist = abs(position.z);
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);

    // fogFactor means the level of fog, or the fog itself according to the varieties of distance

    // second parameter means how light / thin you want the level of fog to be
    // when the fog begin to spread around the camera   (minimum 0.0, maximum 1.0, has to be float)

    // third parameter means how heavy you want the level of fog to be
    // when the fog spread to the far most distance     (minimum 0.0, maximum 1.0, has to be float)
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 shadeColor = phongModel(position, normalize(normal));  // orignally how the objects seems to be without fog effect

    // mix up everything, fog color, original color of objects, and how the distance to the camera effects the level of fog
    vec3 color = mix(Fog.Color, shadeColor, fogFactor);

    // Final output
    // FragColor = vec4(phongSpotModel(position, normalize(normal)), 1.0); // disabled since lab 3.6
    FragColor = vec4(color, 1.0);

    // lab 3.6
}
