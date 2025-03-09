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

/////////////////// Image Processing Techniques - Bloom Effect with Gamma Correction (based on HDR) ///////////////////

// lab 5.4

///// Bloom Effect with Gamma Correction additional information /////


// Lab 5.3
/// HDR with Tone Mapping additional information ///

uniform float AveLum;
uniform float Exposure = 0.35;
uniform float White = 0.928;
// uniform bool DoToneMap = true;

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


// Lab 5.1
/// Edge Detection additional information ///

uniform float LumThresh;    // was called EdgeThreshold in lab 5.1 edge detection

// Relative luminance, constant for the edge to glow 
const vec3 lum = vec3(0.2126, 0.7152, 0.0722);

float luminance(vec3 color)
{
    return dot(lum, color);
    // return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

/* In lab 5.4 it is actually

uniform float LumThresh;    // Luminance threshold

float luminance(vec3 color)
{
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}
*/

/// Edge Detection additional information ///


// Lab 5.2
/// Gaussian Blur additional information ///

uniform float Weight[10];

/// Gaussian Blur additional information ///



layout (binding = 1) uniform sampler2D BlurTex1;
layout (binding = 2) uniform sampler2D BlurTex2;

uniform float PixOffset[10] = float[](0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);

uniform float Gamma;    // gamma correction

///// Bloom Effect with Gamma Correction additional information /////

// when the uniform pass in from outside, decide what logic it would perform
uniform int Pass;

// lab 5.4 go back to functions with return statement
// edge detection needs 2 pass()
// gaussian blur needs 3 pass()
// hdr with tone mapping needs 2 pass()
// bloom effect with gamma correction (based on hdr) needs 5 pass()

// The render pass (lab sheet 5 p.32)
vec4 pass1()    // always the actual object rendering with lighting model
{
    // instead of using HdrColor directly in lab 5.3, a new vec3 is replacing it
    vec3 color = vec3(0.0);
    for (int i = 0; i < 3; i++)
        color += phongModel(i, position, normalize(normal));

        // Gamma correction
        // FragColor = vec4( pow( color, vec3(1.0 / Gamma)), 1.0);

    return vec4(color, 1);
    // return vec4( pow( color, vec3(1.0 / Gamma)), 1.0);
}

// former Lab 5.1 pass2()
vec4 pass2()    // Bright-pass filter (write to BlurTex1) (lab sheet 5 p.33)
{
    vec4 val = texture(HdrTex, TexCoord);

    if( luminance(val.rgb) > LumThresh )

        return val;

    else

        return vec4(0.0);
}

// former Lab 5.2 pass2()
vec4 pass3()    // First blur pass (read from BlurTex1, write to BlurTex2) (lab sheet 5 p.33)
{
    float dy = 1.0 / (textureSize( BlurTex1, 0 )).y;

    vec4 sum = texture(BlurTex1, TexCoord) * Weight[0];

    for( int i = 1; i < 10; i++)
    {
        sum += texture( BlurTex1, TexCoord + vec2(0.0, PixOffset[i]) * dy ) * Weight[i];
        sum += texture( BlurTex1, TexCoord - vec2(0.0, PixOffset[i]) * dy ) * Weight[i];
    }

    return sum;
}

// former Lab 5.2 pass3()
vec4 pass4()    // Second blur pass (read from BlurTex2, write to BlurTex1) (lab sheet 5 p.34)
{
    float dx = 1.0 / (textureSize( BlurTex2, 0 )).x;

    vec4 sum = texture(BlurTex2, TexCoord) * Weight[0];

    for( int i = 1; i < 10; i++)
    {
        sum += texture( BlurTex2, TexCoord + vec2(PixOffset[i], 0.0) * dx ) * Weight[i];
        sum += texture( BlurTex2, TexCoord - vec2(PixOffset[i], 0.0) * dx ) * Weight[i];
    }

    return sum;
}

// former Lab 5.3 pass2(), Apply the tone map to the HDR image & combine it with the blurred bright pass filter
vec4 pass5()    // (Read from BlurTex1 and HdrTex, write to default buffer) (lab sheet 5 p.35)
{
    /////////////// Tone mapping ///////////////

    // Retrieve high-res color from texture
    vec4 color = texture( HdrTex, TexCoord );

    // Convert to XYZ
    vec3 xyzCol = rgb2xyz * vec3(color);
    
    // Convert to xyY
    float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
    vec3 xyYCol = vec3( xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y);

    // Apply the tone mapping operation to the luminance (xyYCol.z or xyzCol.y)
    float L = ( Exposure * xyYCol.z ) / AveLum;
    L = (L * ( 1 + L / (White * White) )) / ( 1 + L );
    // using L = (L * ( 1 - L / (White * White) )) / ( 1 + L ); can revert the color

    // Using the new luminance, convert back to XYZ
    xyzCol.x = ( L * xyYCol.x ) / (xyYCol.y);
    xyzCol.y = L;
    xyzCol.z = ( L * (1 - xyYCol.x - xyYCol.y)) / xyYCol.y;
    
    /* Convert back to RGB and send to output buffer
    if( DoToneMap )
        FragColor = vec4( xyz2rgb * xyzCol, 1.0);
    else
        FragColor = color;
    */

    // Gamma correction
    // FragColor = vec4( pow( color, vec3(1.0 / Gamma)), 1.0);

    // Convert back to RGB
    vec4 toneMapColor = vec4( xyz2rgb * xyzCol, 1.0);

    /////////////// Tone mapping ///////////////

    ///////////// Combine with blurred texture /////////////

    // We want linear filtering on this texture access so that
    // we get additional blurring.
    vec4 blurTex = texture(BlurTex1, TexCoord);

    // Gamma correction
    // FragColor = vec4( pow( color, vec3(1.0 / Gamma)), 1.0);

    return toneMapColor + blurTex;
    // return toneMapColor + blurTex * vec4( pow( color.rgb, vec3(1.0 / Gamma)), 1.0);
    // return toneMapColor + blurTex * vec4( pow( color.xyz, vec3(1.0 / Gamma)), 1.0);
    // return toneMapColor + blurTex + vec4( pow( color.rgb, vec3(1.0 / Gamma)), 1.0);
    // return toneMapColor + blurTex + vec4( pow( color.xyz, vec3(1.0 / Gamma)), 1.0);

    ///////////// Combine with blurred texture /////////////
}

// lab 5.4

/////////////////// Image Processing Techniques - Bloom Effect with Gamma Correction (based on HDR) ///////////////////

// edge detection takes 2 pass()
// gaussian blur takes 3 pass()
// hdr with tone mapping takes 2 pass()
// bloom effect with gamma correction (based on hdr) takes 5 pass()
void main()
{
    // lab 5.4

    vec4 color;

    // an interesting way to do mode changing when rendering different buffers in model(s)
    // Pass in here refer to the level, similar as the lab 3.5 toon shading lab
    if (Pass == 1) color = pass1();
    else if (Pass == 2) color = pass2();
    else if (Pass == 3) color = pass3();
    else if (Pass == 4) color = pass4();
    // else if (Pass == 5) color = pass5();
    else if (Pass == 5) color = vec4( pow( vec3(pass5()), vec3(1.0 / Gamma)), 1.0); // desired solution to lab 5.4

    // FragColor = vec4(phongModel(position, normalize(normal)), 1.0);

    // Gamma correction
    FragColor = color;
    // FragColor = vec4( pow( color.rgb, vec3(1.0 / Gamma)), 1.0);
    // FragColor = pow( color, vec4(1.0 / Gamma));

    // lab 5.4
}
