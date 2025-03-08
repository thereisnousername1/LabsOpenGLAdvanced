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

// lab 5.3

layout (location = 1) out vec3 HdrColor;

// lab 5.3

/////////////////// Phong / Blinn-Phong switchable shading model and necessary information ///////////////////

uniform struct LightInfo
{
    vec4 Position;  // could be static or moving, depends on logic in the program

    vec3 Ld;    // diffuse light intensity
    vec3 La;    // ambient light intensity
    vec3 Ls;    // specular light intensity

// }Light;  // Disabled since lab 5.3
}lights[3]; // multiple lights logic

uniform struct MaterialInfo
{
    vec3 Kd;    // diffuse material reflected color
    vec3 Ka;    // ambient material reflected color
    vec3 Ks;    // specular material reflected color

    float shininess;

}Material;

// in the video Blinn-Phong is used, but in my study I want to keep both

vec3 phongModel (int light, vec3 position, vec3 n)   // multiple directional lights
// vec3 phongModel (vec3 position, vec3 n)  // turn to multiple directional lights since lab 5.3
{

//// diffuse lighting is directional lighting
    vec3 diffuse = vec3(0.0);

//// specular lighting is meant to represent glare upon an object's surface
    vec3 specular = vec3(0.0);

    // combined the actual texture with the coordinate
    // to output the color of that texture
    // vec3 texColor = texture(Tex, TexCoord).rgb;

//// ambient lighting is a constant & omnipresent light source
    // vec3 ambient = Light.La * Material.Ka;           // Disabled since lab 5.3
    vec3 ambient = lights[light].La * Material.Ka;      // modified for multiple lights since lab 5.3
    // vec3 ambient = Light.La * Material.Ka * texColor;   // this way it combined the material reflect color and texture color
    // vec3 ambient = Light.La * texColor;   // no needed since lab 5.1

    // light direction
    // calculate the direction from the light position to the vertex point with normalize
    // vec3 s = normalize(Light.Position.xyz - position);       // find out s vector
    vec3 s = normalize(lights[light].Position.xyz - position);  // specialized multiple lights formula

    // According to OpenGL SuperBible p.569 (CH13 - Diffuse Light), this dot product caould be a negative value
    // max() is placed to prevent such situation, it will justify the negative part to zero
    float sDotN = max(dot(s, n), 0.0);    // calculate the dot product of normalized normal and light direction

    // experiment when we don't use max()
    // float sDotN = dot(s, n);

    // diffuseColor = diffuseMaterial * DiffuseLight * the dot product from normal and light direction
    // diffuse = Light.Ld * Material.Kd * sDotN;        // Disabled since lab 5.3
    diffuse = lights[light].Ld * Material.Kd * sDotN;   // modified for multiple lights since lab 5.3
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
        vec3 v = normalize(-position.xyz);

        /// SWITCHABLE ///

        // reflection -> Phong Shading
        vec3 r = reflect(-s, n);

        // calculate the half vector -> Blinn-Phong Shading
        // vec3 h = normalize(s + v);

        /// SWITCHABLE ///
    
        // original material color setting in scenebasic_uniform
        //// For Directional Light, use lights[light].Ls in multiple lights scenario
        // final spec                            eye reflection angle
        // specular = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);        // reflection -> Phong Shading
        specular = lights[light].Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);   // specialized multiple lights formula

        // specular = Light.Ls * Material.Ks * pow(max(dot(h, n), 0.0), Material.shininess);            // calculate the half vector -> Blinn-Phong Shading
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

/////////////////// Image Processing Techniques - HDR with Tone Mapping ///////////////////

// lab 5.3

/// HDR with Tone Mapping additional information ///

uniform float AveLum;
uniform float Exposure = 0.35;
uniform float White = 0.928;
uniform bool DoToneMap = true;

layout (binding = 0) uniform sampler2D HdrTex;  // former Tex / Tex1

// XYZ / RGB conversion matrices from:

// http://www.brucelindbloom.com/index.htm?Eqn_RGB_XYZ_Matrix.html

uniform mat3 rgb2xyz = mat3(
0.4124564, 0.2126729, 0.0193339,
0.3575761, 0.7151522, 0.1191920,
0.1804375, 0.0721750, 0.9503041 
);

uniform mat3 xyz2rgb = mat3(
3.2404542, -0.9692660, 0.0556434,
-1.5371385, 1.8760108, -0.2040259,
-0.4985314, 0.0415560, 1.0572252
);

/// HDR with Tone Mapping additional information ///

// when the uniform pass in from outside, decide what logic it would perform
uniform int Pass;

// since lab 5.3 they become void function
// edge detection needs 2 pass()
// gaussian blur needs 3 pass()
// hdr with tone mapping needs 2 pass()
void pass1()    // always the actual object rendering with lighting model
{
    // Compute shading and store result in high-res framebuffer
    HdrColor = vec3(0.0);
    for (int i = 0; i < 3; i++)
        HdrColor += phongModel(i, position, normalize(normal));
}

void pass2()    // this pass computes the sum of the luminance of all pixels
{

    // Retrieve high-res color from texture
    vec4 color = texture( HdrTex, TexCoord );

    // Convert to XYZ
    vec3 xyzCol = rgb2xyz * vec3(color);
    
    // Convert to xyY
    float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
    vec3 xyYCol = vec3( xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y);

    // Apply the tone mapping operation to the luminance (xyYCol.z or xyzCol.y)
    float L = ( Exposure * xyYCol.z) / AveLum;
    L = (L * ( 1 + L / (White * White) )) / ( 1 + L );
    // using L = (L * ( 1 - L / (White * White) )) / ( 1 + L ); can revert the color

    // Using the new luminance, convert back to XYZ
    xyzCol.x = ( L * xyYCol.x ) / (xyYCol.y);
    xyzCol.y = L;
    xyzCol.z = ( L * (1 - xyYCol.x - xyYCol.y)) / xyYCol.y;
    
    // Convert back to RGB and send to output buffer
    if( DoToneMap )
        FragColor = vec4( xyz2rgb * xyzCol, 1.0);
    else
        FragColor = color;
}

// lab 5.3

/////////////////// Image Processing Techniques - HDR with Tone Mapping ///////////////////

// edge detection takes 2 pass()
// gaussian blur takes 3 pass()
// hdr with tone mapping takes 2 pass()
void main()
{
    // lab 5.2

    // an interesting way to do mode changing when rendering different buffers in model(s)
    // Pass in here refer to the level, similar as the lab 3.5 toon shading lab
    if (Pass == 1) pass1();
    else if (Pass == 2) pass2();
    // FragColor handled in pass()

    // FragColor = vec4(phongModel(position, normalize(normal)), 1.0);

    // lab 5.2
}
