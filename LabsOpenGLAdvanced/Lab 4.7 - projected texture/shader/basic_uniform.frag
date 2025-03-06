#version 460

///
/// In fragment shader
/// for how many variables are displayed as in 
/// same variables should come out from vertex shader
///

// intended to be handled by program, less likely needed to be modified
in vec3 normal;     // renamed to EyeNormal since lab 4.7
in vec3 position;   // renamed to EyePosition since lab 4.7

// since lab 4.7 only single texture is needed

// in vec2 TexCoord;   // renamed to ProjTexCoord since lab 4.7
in vec4 TexCoord;

// according to https://blog.mmorpgtycoon.com/post/opengl-texture-inconsistent/
// lets assume the binding word related to the "slot number" of a texture unit
// so without actually using the relative texture unit with the number binded here
// it gone wrong and causing glitchy texture
//
// also maybe the type have to be the same as well
//
// actual texture to be imported from program
layout (binding = 0) uniform sampler2D Tex1;    // renamed to ProjectorTex since lab 4.7

// lab 4.7

// Final Output
layout (location = 0) out vec4 FragColor;

/////////////////// Phong / Blinn-Phong switchable shading model and necessary information ///////////////////

uniform struct LightInfo
{
    vec4 Position;  // could be static or moving, depends on logic in the program

    vec3 Ld;    // diffuse light intensity
    vec3 La;    // ambient light intensity
    vec3 Ls;    // specular light intensity

}Light;
// }lights[3]; // multiple lights logic

uniform struct MaterialInfo
{
    vec3 Kd;    // diffuse material reflected color
    vec3 Ka;    // ambient material reflected color
    vec3 Ks;    // specular material reflected color

    float shininess;

}Material;

// in the video Blinn-Phong is used, but in my study I want to keep both

// vec3 phongModel (int light, vec3 position, vec3 n)   // multiple directional lights
vec3 phongModel (vec3 position, vec3 n)
{

//// diffuse lighting is directional lighting
    vec3 diffuse = vec3(0.0);

//// specular lighting is meant to represent glare upon an object's surface
    vec3 specular = vec3(0.0);

    // lab 4.7

    // mapping as single texture
    //                      base texture
    // vec3 texColor = texture(Tex1, TexCoord).rgb; // not needed since lab 4.7

//// ambient lighting is a constant & omnipresent light source
    vec3 ambient = Light.La * Material.Ka;   // enabled since lab 4.7
    // vec3 ambient = Light.La * Material.Ka * texColor;   // this way it combined the material reflect color and texture color
    // vec3 ambient = Light.La * texColor;   // desired way in lab 4.1

    // lab 4.7

    // light direction
    // calculate the direction from the light position to the vertex point with normalize
    vec3 s = normalize(Light.Position.xyz - position);    // find out s vector

    // According to OpenGL SuperBible p.569 (CH13 - Diffuse Light), this dot product caould be a negative value
    // max() is placed to prevent such situation, it will justify the negative part to zero
    float sDotN = max(dot(s, n), 0.0);    // calculate the dot product of normalized normal and light direction

    // experiment when we don't use max()
    // float sDotN = dot(s, n);

    // lab 4.1

    // diffuseColor = diffuseMaterial * DiffuseLight * the dot product from normal and light direction
    diffuse = Light.Ld * Material.Kd * sDotN;    // enabled since lab 4.7
    // diffuse = Light.Ld * Material.Kd * texColor * sDotN;   // material is now combined with texture
    // diffuse = Light.Ld * texColor * sDotN;   // desired way in lab 4.1

    // lab 4.1

    //// In the lab pdf, diffuse intensity and specular intensity is combined
    //// they are meant to be multiply in the final output
    //// so originally diffuse in here will only combined Material.Kd and sDotN together

    // by default I choose Phong instead of Blinn-Phong
    if (sDotN > 0.0)
    {
        // p.569 of opengl superbible
        // eye normal
        vec3 v = normalize(-position.xyz);

        /// SWITCHABLE ///

        // reflection -> Phong Shading
        vec3 r = reflect(-s, n);

        // calculate the half vector -> Blinn-Phong Shading
        // vec3 h = normalize(s + v);

        /// SWITCHABLE ///
    
        // desired way in lab 4.1
        // original material color setting in scenebasic_uniform
        //// For Directional Light, use lights[light].Ls in multiple lights scenario
        // final spec                            eye reflection angle
        specular = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);   // reflection -> Phong Shading
        // specular = Light.Ls * Material.Ks * pow(max(dot(h, n), 0.0), Material.shininess);   // calculate the half vector -> Blinn-Phong Shading

        // mixed texColor with other factor
        // specular = Light.Ls * Material.Ks * texColor * pow(max(dot(r, v), 0.0), Material.shininess);   // reflection -> Phong Shading
        // specular = Light.Ls * Material.Ks * texColor * pow(max(dot(h, n), 0.0), Material.shininess);   // calculate the half vector -> Blinn-Phong Shading

        // replace material reflect color with texColor
        // specular = Light.Ls * texColor * pow(max(dot(r, v), 0.0), Material.shininess);   // reflection -> Phong Shading
        // specular = Light.Ls * texColor * pow(max(dot(h, n), 0.0), Material.shininess);   // calculate the half vector -> Blinn-Phong Shading
    
        /// SWITCHABLE ///

        //// In the lab pdf, diffuse intensity and specular intensity is combined
        //// they are meant to be multiply in the final output
        //// so originally specular in here will only combined Material.Ks and
        //// pow(max(dot(r, v), 0.0), Material.shininess) together

    }

    // final ouput
    return diffuse + ambient + specular;

    //// Originally lights[light].L are meant to be merge here
    //// the code looks like this
    //// return ambient + lights[light].L * (diffuse + specular);
    //// for the lights[light].L only apply to diffuse and specular
    //// while ambient remains separated, cuz it is unique in the scene
}

/////////////////// Phong / Blinn-Phong switchable shading model and necessary information ///////////////////

void main()
{
    // lab 4.7

    vec3 color = phongModel(position, normalize(normal));

    vec3 projTexColor = vec3(0.0);

    // Basic output logic
    // FragColor = vec4(phongModel(position, normalize(normal)), 1.0);

    // if (ProjectCoord.z > 0.0)
    //     ProjTexColor = textureProj(ProjectorTex, ProjTexCoord).rgb;

    if (TexCoord.z > 0.0)
        projTexColor = textureProj(Tex1, TexCoord).rgb;

    FragColor = vec4(color + projTexColor * 0.5, 1.0);

    /* this is not needed since no alpha clipping = you won't see the inner -> no need to do condition checking
    if (gl_FrontFacing)
    {
        FragColor = vec4(phongModel(position, normalize(normal)), 1.0);
    }
    else
    {
        FragColor = vec4(phongModel(position, normalize(-normal)), 1.0);
    }
    */

    // lab 4.7
}
