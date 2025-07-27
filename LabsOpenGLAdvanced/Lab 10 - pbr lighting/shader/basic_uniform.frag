#version 460

///
/// In fragment shader
/// for how many variables are displayed as in 
/// same variables should come out from vertex shader
///

const float PI = 3.14159265358979323846;

// intended to be handled by program, less likely needed to be modified
in vec3 position;   // lab 9.5
// in vec4 position;   // lab 9.3
in vec3 normal;

// Final Output
layout (location = 0) out vec4 FragColor;

/////////////////// Phong / Blinn-Phong switchable shading model and necessary information ///////////////////

uniform struct LightInfo
{
    vec4 Position;  // could be static or moving, depends on logic in the program

    vec3 Ld;    // diffuse light intensity, not in use in lab 10
    vec3 La;    // ambient light intensity, not in use in lab 10
    vec3 Ls;    // specular light intensity, not in use in lab 10

    vec3 L;    // all light intensity

// }Light;
}lights[3]; // multiple lights logic

uniform struct MaterialInfo
{
    //vec3 Kd;    // diffuse material reflected color
    //vec3 Ka;    // ambient material reflected color
    //vec3 Ks;    // specular material reflected color

    // float shininess; // it turns to roughness in lab 10

    float Rough;    // Roughness
    bool Metal;     // Metallic (true) or dielectric (false)
    vec3 Color;     // Diffuse color for dielectrics, f0 for metallic

}Material;

float ggxDistribution( float nDotH )
{
    float alpha2  = Material.Rough * Material.Rough * Material.Rough * Material.Rough;
    float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

float geomSmith (float dotProd)
{
    float k = (Material.Rough + 1.0) * (Material.Rough + 1.0) / 8.0;
    float denom = dotProd * (1 - k) + k;
    return 1.0 / denom;
}

vec3 schlickFresnel( float lDotH ) {
    vec3 f0 = vec3(0.04);
    if ( Material.Metal )
    {
        f0 = Material.Color;
    }
    return f0 + (1 - f0) * pow(1.0 - lDotH, 5);
}

/* former phongModel
vec3 phongModel (int light, vec3 position, vec3 n)
{

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    vec3 ambient = lights[light].La * Material.Color;
    vec3 s = normalize(lights[light].Position.xyz - position);

    float sDotN = max(dot(s, n), 0.0);

    diffuse = lights[light].Ld * Material.Color * sDotN;

    if (sDotN > 0.0)
    {
        vec3 v = normalize(-position.xyz);

        /// SWITCHABLE ///

        // reflection -> Phong Shading
        vec3 r = reflect(-s, n);

        // calculate the half vector -> Blinn-Phong Shading
        // vec3 h = normalize(s + v);
        specular = lights[light].Ls * Material.Color * pow(max(dot(r, v), 0.0), Material.Rough);   // specialized multiple lights formula
        // specular = lights[light].Ls * Material.Color * pow(max(dot(h, n), 0.0), Material.shininess);   // specialized multiple lights formula
    }

    return diffuse + ambient + specular;
}
*/

vec3 microfacetModel( int lightIdx, vec3 position, vec3 n ) {
    vec3 diffuseBrdf = vec3(0.0);   // Metallic
    if( !Material.Metal )
    {
        diffuseBrdf = Material.Color;
    }
    
    //                     it is light[lightIdx].L in the lab, I guess maybe I should go ( lights[lightIdx].Ld + lights[lightIdx].La + lights[lightIdx].Ls ) / 3?
    vec3 l = vec3(0.0), lightI = ( lights[lightIdx].Ld + lights[lightIdx].La + lights[lightIdx].Ls ) / 3;
    
    if( lights[lightIdx].Position.w == 0.0 ) // Directional light
    {
        l = normalize(lights[lightIdx].Position.xyz);
    }
    else                                     // Positional light
    {
        l = lights[lightIdx].Position.xyz - position;
        float dist = length(l);
        l = normalize(l);
        lightI /= (dist * dist);
    }

    vec3 v = normalize( -position );

    vec3 h = normalize( v + l );    // I guess it is exactly the blinnPhong method in here
    vec3 r = reflect(-l, n);        // I am introducing my method for this situation

    // using l and h (blinnPhong)
    float nDotH = dot( n, h );
    float lDotH = dot( l, h );

    // using l and r (Phong)
    float nDotR = dot( n, r );
    float lDotR = dot( l, r );

    float nDotL = max( dot( n, l), 0.0 );
    float nDotV = dot( n, v );

    // using l and h (blinnPhong)
    vec3 specBrdf1 = 0.25 * ggxDistribution(nDotH) * schlickFresnel(lDotH) * geomSmith(nDotL) * geomSmith(nDotV);

    // using l and r (Phong)
    vec3 specBrdf2 = 0.25 * ggxDistribution(nDotR) * schlickFresnel(lDotR) * geomSmith(nDotL) * geomSmith(nDotV);

    /// SWITCHABLE ///

    return (diffuseBrdf + PI * specBrdf1) * lightI * nDotL;
    // return (diffuseBrdf + PI * specBrdf2) * lightI * nDotL;  // weird, turns out it is not for phong lighting, but instead it will just burst out, i have similar problem in gamma lab
}

void main ()
{
    vec3 sum = vec3(0);
    vec3 n = normalize(normal);

    for( int i = 0; i < 3; i++ )
    {
        sum += microfacetModel(i, position, n);
    }

    // Gamma
    sum = pow( sum, vec3(1.0/2.2) );

    FragColor = vec4(sum, 1);
}