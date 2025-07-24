#version 460

///
/// In fragment shader
/// for how many variables are displayed as in 
/// same variables should come out from vertex shader
///

// intended to be handled by program, less likely needed to be modified
in vec3 normal;
in vec3 position;

// lab 8.1

in vec4 ShadowCoord;

uniform sampler2DShadow ShadowMap;

// lab 8.3

uniform sampler3D OffsetTex;

uniform float Radius;

uniform vec3 OffsetTexSize; // (width, height, depth)

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

    // lab 4.1

    // combined the actual texture with the coordinate
    // to output the color of that texture
    // vec3 texColor = texture(Tex1, TexCoord).rgb;

//// ambient lighting is a constant & omnipresent light source
    vec3 ambient = Light.La * Material.Ka;   // replaced for combining texture
    // vec3 ambient = Light.La * Material.Ka * texColor;   // this way it combined the material reflect color and texture color
    // vec3 ambient = Light.La * texColor;   // desired way in lab 4.1

    // lab 4.1

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
    diffuse = Light.Ld * Material.Kd * sDotN;    // disabled in lab 4
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

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
    vec3 ambient = Light.La * Material.Ka;
    vec3 diffAndSpec = phongModel(position, normalize(normal));

    /* former lab 8.2 lines
    // Don't text points behind the light source.
    if (ShadowCoord.z >= 0)
    {
        // Sum contributions from 4 texels around ShadowCoord
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1, -1));
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1, 1)); 
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1, 1));
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1, -1));
        shadow = sum * 0.25;
    }
    */

    // lab 8.3

    // look up the texels nearby
    float sum = 0;
    float shadow = 1.0;
    
    ivec3 offsetCoord;
    offsetCoord.xy = ivec2( mod( gl_FragCoord.xy, OffsetTexSize.xy ) );
    
    int samplesDiv2 = int(OffsetTexSize.z);
    vec4 sc = ShadowCoord;

    // Don't test points behind the light source.
    if( sc.z >= 0 ) {
        for( int i = 0 ; i < 4; i++ )
        {
            offsetCoord.z = i;
            vec4 offsets = texelFetch(OffsetTex, offsetCoord, 0) * Radius * ShadowCoord.w;

            sc.xy = ShadowCoord.xy + offsets.xy;
            sum += textureProj(ShadowMap, sc);
            sc.xy = ShadowCoord.xy + offsets.zw;
            sum += textureProj(ShadowMap, sc);
        }
        shadow = sum / 8.0;

        if( shadow != 1.0 && shadow != 0.0 )
        {
            for ( int i = 4; i < samplesDiv2; i++ )
            {
                offsetCoord.z = i;
                vec4 offsets = texelFetch(OffsetTex, offsetCoord, 0) * Radius * ShadowCoord.w;

                sc.xy = ShadowCoord.xy + offsets.xy;
                sum += textureProj(ShadowMap, sc);
                sc.xy = ShadowCoord.xy + offsets.zw;
                sum += textureProj(ShadowMap, sc);
            }
        shadow = sum / float(samplesDiv2 * 2.0);
        }
    }

    // lab 8.3

    // if the fragment is in shadow, use ambient light only
    FragColor = vec4(diffAndSpec * shadow + ambient, 1.0);
    
    // gamma correct
    FragColor = pow( FragColor, vec4(1.0 / 2.2) );
}

subroutine (RenderPassType)
void recordDepth()
{
    // do nothing, depth written automatically
}

void main()
{
    // This will call either shadeWithShadow or recordDepth
    RenderPass();
    //FragColor = vec4(phongModel(position, normalize(normal)), 1.0);
}
