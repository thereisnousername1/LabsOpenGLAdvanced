#version 460

///
/// In fragment shader
/// for how many variables are displayed as in 
/// same variables should come out from vertex shader
///

// intended to be handled by program, less likely needed to be modified
in vec3 normal;         // rename to ViewDir in Lab 4 part 5 video
in vec3 position;       // rename to LightDir in Lab 4 part 5 video

// lab 4.1 - single texture

in vec2 TexCoord;

// according to https://blog.mmorpgtycoon.com/post/opengl-texture-inconsistent/
// lets assume the binding word related to the "slot number" of a texture unit
// so without actually using the relative texture unit with the number binded here
// it gone wrong and causing glitchy texture
//
// also maybe the type have to be the same as well
//
// actual texture to be imported from program
layout (binding = 0) uniform sampler2D Tex1;    // in the lab it is renamed

// lab 4.1

// lab 4.2 multiple texture

layout (binding = 1) uniform sampler2D Tex2;    // in the lab it is renamed

// lab 4.2 multiple texture

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

    // lab 4.2 - color now handled by texture imported (by default)

    // combined the actual texture with the coordinate
    // to output the color of that texture
    vec3 texColor = texture(Tex1, TexCoord).rgb;     // single texture

    /*  disabled in lab 4.3 alpha discard
                                                        // multiple texture (jpg + png(transparent))
                                                        // mixed up 2 texture to display on 1 obj
    vec4 texColor1 = texture(Tex1, TexCoord);
    vec4 texColor2 = texture(Tex2, TexCoord);

    // it means mixing texture1  , texture2 , using moss(texture 2).alpha
    vec3 texColor = mix(texColor1.rgb, texColor2.rgb, texColor2.a);
    */

                                                        // alpha discard, following logic in main()
                                                        // clipping off pixels on texture 1
                                                        // according to the texture 2 alpha channel
    // mapping as single texture
    //                      base texture
    // vec3 texColor = texture(Tex1, TexCoord).rgb;

//// ambient lighting is a constant & omnipresent light source
    // vec3 ambient = Light.La * Material.Ka;   // replaced for combining texture
    // vec3 ambient = Light.La * Material.Ka * texColor;   // this way it combined the material reflect color and texture color
    vec3 ambient = Light.La * texColor;   // desired way in lab 4.1

    // lab 4.2

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
    // diffuse = Light.Ld * Material.Kd * sDotN;    // disabled in lab 4
    // diffuse = Light.Ld * Material.Kd * texColor * sDotN;   // material is now combined with texture
    diffuse = Light.Ld * texColor * sDotN;   // desired way in lab 4.1

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

vec3 phongModel (vec3 n)
{
    vec3 diffuse = vec3(0.0), specular = vec3(0.0);

    vec3 texColor = texture(Tex1, TexCoord).rgb;    // base texture

    vec3 ambient = Light.La * texColor;

    // light direction
    //// this part is different from regular phong model
    vec3 s = normalize(position);

    // According to OpenGL SuperBible p.569 (CH13 - Diffuse Light), this dot product caould be a negative value
    // max() is placed to prevent such situation, it will justify the negative part to zero
    float sDotN = max(dot(s, n), 0.0);    // calculate the dot product of normalized normal and light direction

    diffuse = Light.Ld * texColor * sDotN;

    // by default I choose Phong instead of Blinn-Phong
    if (sDotN > 0.0)
    {
        //// this part is different from regular phong model
        vec3 v = normalize(normal);

        /// SWITCHABLE ///

        // reflection -> Phong Shading
        vec3 r = reflect(-s, n);

        // calculate the half vector -> Blinn-Phong Shading
        // vec3 h = normalize(s + v);

        /// SWITCHABLE ///
    
        // final spec                            eye reflection angle
        specular = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);   // reflection -> Phong Shading
        // specular = Light.Ls * Material.Ks * pow(max(dot(h, n), 0.0), Material.shininess);   // calculate the half vector -> Blinn-Phong Shading
    
        /// SWITCHABLE ///

    }
    // final ouput
    return diffuse + ambient + specular;
}

/////////////////// Phong / Blinn-Phong switchable shading model and necessary information ///////////////////

void main()
{
    // different mapping methods are done in main()
    // usually by calling texture() to texture(verb.) the Tex2 in different way
    // most importantly, these methods have to do something with texColor
    // for texColor with affect the FragColor by taking part in ambient and diffuse lights

    // lab 4.4 - normal map - reference to OpenGL SuperBible p.585(Ch. 13)

    vec3 normMap = texture(Tex2, TexCoord).xyz;
    normMap.xy = 2.0 * normMap.xy - 1.0;

    // it should be used only when material has its own normal mapping
    // FragColor = vec4(phongModel(normalize(normMap)), 1.0);   // lab 4.4 solution

    // lab 4.4

    // Basic output logic
    // FragColor = vec4(phongModel(position, normalize(normal)), 1.0);

    // rough decision model -> comply with vertex shader
    // decide using normal map or program generate for user

    // Advanced output logic
    // if it is facing the front
    if (gl_FrontFacing)
    {
        // FragColor = vec4(phongModel(position, normalize(normal)), 1.0);
        FragColor = vec4(phongModel(normalize(normMap)), 1.0);
    }
    else
    {
        // FragColor = vec4(phongModel(position, normalize(-normal)), 1.0);
        FragColor = vec4(phongModel(normalize(-normMap)), 1.0);
    }

    /* lab 4.3 - alpha map, disabled in lab 4.4

    // clipping the base texture (Tex1 rgb) accoding to 
    // the alpha channel of the alpha texture (Tex2 a)
    // in this case alpha is like a mask of the first texture
    //                      alpha map texture

    
    vec4 alphaMap = texture(Tex2, TexCoord);

    if (alphaMap.a < 0.15f)
    {
        discard;
    }
    else
    {
        // if it is facing the front
        if (gl_FrontFacing)
        {
            FragColor = vec4(phongModel(position, normalize(normal)), 1.0);
        }
        else
        {
            FragColor = vec4(phongModel(position, normalize(-normal)), 1.0);
        }
    }

    */ // lab 4.3
}
