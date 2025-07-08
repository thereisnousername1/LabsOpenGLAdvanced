#version 460

///
/// In fragment shader
/// for how many variables are displayed as in 
/// same variables should come out from vertex shader
///

// intended to be handled by program, less likely needed to be modified
in vec3 normal;
in vec3 position;

// lab 5.3

in vec2 TexCoord;   // added since lab 5.3

// lab 5.3

// lab 5.3 - Tex not in use

// according to https://blog.mmorpgtycoon.com/post/opengl-texture-inconsistent/
// lets assume the binding word related to the "slot number" of a texture unit
// so without actually using the relative texture unit with the number binded here
// it gone wrong and causing glitchy texture
//
// also maybe the type have to be the same as well
//
// actual texture to be imported from program
// layout (binding = 0) uniform sampler2D Tex1; // refer to single texture rendering

// without binding = 0
// uniform sampler2D RenderTex; // in the lab standard solution it is called RenderTex
                                // for my own study I will rename it as Tex,
                                // so that I can compare it with Lab 5 solutions
// uniform sampler2D Tex;   // I named it Tex because all the sampler2D uniform in previous lab named Tex<number>

// lab 5.3

// Final Output
layout (location = 0) out vec4 FragColor;

// layout (location = 1) out vec3 HdrColor;

// lab 5.5

layout (location = 1) out vec3 PositionData;
layout (location = 2) out vec3 NormalData;
layout (location = 3) out vec3 ColorData;

// lab 5.5

/////////////////// Phong / Blinn-Phong switchable shading model and necessary information ///////////////////

uniform struct LightInfo
{
    vec4 Position;  // could be static or moving, depends on logic in the program

    vec3 Ld;    // diffuse light intensity
    vec3 La;    // ambient light intensity
    vec3 Ls;    // specular light intensity

}Light;
// }lights[3]; // multiple lights logic, disabled since lab 5.5

uniform struct MaterialInfo
{
    vec3 Kd;    // diffuse material reflected color
    
    // details modified from lab 5.5

    vec3 Ka;    // ambient material reflected color
    vec3 Ks;    // specular material reflected color

    float shininess;

    // lab 5.5

}Material;

// from lab 5.5, a different model is used to calculate only specular lighting in this fragment shader
vec3 diffuseModel (vec3 pos, vec3 norm, vec3 diff)
{
    vec3 s = normalize(vec3(Light.Position) - pos);
    float sDotN = max(dot(s, norm), 0.0);
    return Light.Ld*diff*sDotN;
}


// but in my study I want to keep both

// vec3 phongModel (int light, vec3 position, vec3 n)   // multiple directional lights
// vec3 phongModel (vec3 position, vec3 n)  // turn to single directional light since lab 5.5
vec3 phongModel (vec3 pos, vec3 norm, vec3 diffColor)  // diffColor is introduced in lab 5.5
{

//// diffuse lighting is directional lighting
    vec3 diffuse = vec3(0.0);

//// specular lighting is meant to represent glare upon an object's surface
    vec3 specular = vec3(0.0);

    // combined the actual texture with the coordinate
    // to output the color of that texture
    // vec3 texColor = texture(Tex, TexCoord).rgb;

//// ambient lighting is a constant & omnipresent light source
    vec3 ambient = Light.La * Material.Ka;                 // enabled since lab 5.5
    // vec3 ambient = lights[light].La * Material.Ka;      // modified for multiple lights since lab 5.3
    // vec3 ambient = Light.La * Material.Ka * texColor;   // this way it combined the material reflect color and texture color
    // vec3 ambient = Light.La * texColor;   // no needed since lab 5.1

    // light direction
    // calculate the direction from the light position to the vertex point with normalize
    // vec3 s = normalize(Light.Position.xyz - position);          // find out s vector
    vec3 s = normalize(Light.Position.xyz - pos);                  // modified for lab 5.5 deferred shading
    // vec3 s = normalize(lights[light].Position.xyz - position);  // specialized multiple lights formula

    // According to OpenGL SuperBible p.569 (CH13 - Diffuse Light), this dot product caould be a negative value
    // max() is placed to prevent such situation, it will justify the negative part to zero
    // float sDotN = max(dot(s, n), 0.0);    // calculate the dot product of normalized normal and light direction
    float sDotN = max(dot(s, norm), 0.0);    // modified for lab 5.5 deferred shading

    // experiment when we don't use max()
    // float sDotN = dot(s, n);

    // diffuseColor = diffuseMaterial * DiffuseLight * the dot product from normal and light direction
    // diffuse = Light.Ld * Material.Kd * sDotN;         // enabled since lab 5.5
    diffuse = Light.Ld * diffColor * sDotN;              // modified for lab 5.5 deferred shading
    // diffuse = lights[light].Ld * Material.Kd * sDotN;   // modified for multiple lights since lab 5.3
    // diffuse = Light.Ld * Material.Kd * texColor * sDotN;   // material is now combined with texture
    // diffuse = Light.Ld * texColor * sDotN;   // no needed since lab 5.1

    //// In the lab pdf, diffuse intensity and specular intensity is combined
    //// they are meant to be multiply in the final output
    //// so originally diffuse in here will only combined Material.Kd and sDotN together

    // by default I choose Phong instead of Blinn-Phong
    if (sDotN > 0.0)
    {
        // p.569 of opengl superbible
        // eye normal
        // vec3 v = normalize(-position.xyz);
        vec3 v = normalize(-pos.xyz);   // modified for lab 5.5 deferred shading

        /// SWITCHABLE ///

        // reflection -> Phong Shading
        // vec3 r = reflect(-s, n);
        vec3 r = reflect(-s, norm);     // modified for lab 5.5 deferred shading

        // calculate the half vector -> Blinn-Phong Shading
        // vec3 h = normalize(s + v);

        /// SWITCHABLE ///
    
        // original material color setting in scenebasic_uniform
        //// For Directional Light, use lights[light].Ls in multiple lights scenario
        // final spec                            eye reflection angle
        specular = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);              // reflection -> Phong Shading
        // specular = lights[light].Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);   // specialized multiple lights formula

        // specular = Light.Ls * Material.Ks * pow(max(dot(h, norm), 0.0), Material.shininess);            // calculate the half vector -> Blinn-Phong Shading
        // specular = lights[light].Ls * Material.Ks * pow(max(dot(h, n), 0.0), Material.shininess);   // specialized multiple lights formula

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

/////////////////// Image Processing Techniques - Deferred Shading ///////////////////

// lab 5.5

// Bloom Effect with Gamma Correction additional info removed from lab 5.5

layout (binding = 0) uniform sampler2D PositionTex;  // former Tex / Tex1 / HdrTex

// XYZ / RGB conversion matrices removed from lab 5.5

// HDR with Tone Mapping additional info removed from lab 5.5

// Edge detection removed from lab 5.5

// Gaussian Blur info removed from lab 5.5

// Lab 5.2

layout (binding = 1) uniform sampler2D NormalTex;    // former BlurTex1
layout (binding = 2) uniform sampler2D ColorTex;    // former BlurTex1


uniform float Gamma;    // gamma correction

///// Bloom Effect with Gamma Correction additional information /////

// when the uniform pass in from outside, decide what logic it would perform
uniform int Pass;

// edge detection needs 2 pass()
// gaussian blur needs 3 pass()
// hdr with tone mapping needs 2 pass()
// bloom effect with gamma correction (based on hdr) needs 5 pass()
// deferred shading needs 2 pass() and revert everythming written in the past

void pass1()
{
    PositionData = position;
    NormalData = normalize(normal);
    ColorData = Material.Kd;
}

// the render pass (lab sheet 5 p.44)
void pass2()    // Read from the g-buffer, evaluate the reflection model & produce a final colour for each pixel... retrieves the position, normal & colour (lab sheet 5 p.44-45)
{
    vec3 pos = vec3( texture( PositionTex, TexCoord ) );
    vec3 norm = vec3( texture( NormalTex, TexCoord ) );
    vec3 diffColor = vec3( texture( ColorTex, TexCoord ) );

    // in the lab sheet 5 it mentioned the model can switch between diffuseModel and phongModel
    // FragColor = vec4(pow(vec3(diffuseModel( pos, norm, diffColor )), vec3( 1.0/Gamma )), 1.0 );
    FragColor = vec4(pow(vec3(phongModel( pos, norm, diffColor )), vec3( 1.0/Gamma )), 1.0 );
}

// lab 5.5

/////////////////// Image Processing Techniques - Deferred Shading ///////////////////

// edge detection takes 2 pass()
// gaussian blur takes 3 pass()
// hdr with tone mapping takes 2 pass()
// bloom effect with gamma correction (based on hdr) takes 5 pass()
void main()
{
    // lab 5.5

    // an interesting way to do mode changing when rendering different buffers in model(s)
    // Pass in here refer to the level, similar as the lab 3.5 toon shading lab
    if (Pass == 1) 
        pass1();
    else if (Pass == 2) 
        pass2();

    // lab 5.5
}
