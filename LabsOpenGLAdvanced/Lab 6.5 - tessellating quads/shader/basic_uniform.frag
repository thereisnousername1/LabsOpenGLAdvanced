#version 460

///
/// From lab 6 the main focus switch to geometry shader
/// fragment shader will be simplify and phongmodel(or any other parts in previous lab)
/// will be added only when it's necessary for to use or commenting
/// Improvement of readability
///

/* no need in lab 6.4

struct LightInfo
{
    vec4 Position;  // could be static or moving, depends on logic in the program

    vec3 Ld;    // diffuse light intensity
    vec3 La;    // ambient light intensity
    vec3 Ls;    // specular light intensity

};
uniform LightInfo Light;

struct MaterialInfo
{
    vec3 Kd;    // diffuse material reflected color
    vec3 Ka;    // ambient material reflected color
    vec3 Ks;    // specular material reflected color

    float shininess;    // Specular shininess factor

};
uniform MaterialInfo Material;

*/

uniform vec4 LineColor;

uniform float LineWidth;    // added since lab 6.5

uniform vec4 QuadColor;     // added since lab 6.5

noperspective in vec3 EdgeDistance; // added since lab 6.5

/* no need in lab 6.4

// in variables coming from geometry shader
in vec3 GPosition;
in vec3 GNormal;

flat in int GIsEdge;

*/

// out color
layout(location = 0) out vec4 FragColor;

/*
// toon shading levels
const int levels = 3;
const float scaleFactor = 1.0 / levels;

// it was toonShade() in the lab 6.3
// in the video Blinn-Phong is used, but in my study I want to keep both

// vec3 phongModel (int light, vec3 position, vec3 n)   // multiple directional lights
// vec3 phongModel (vec3 position, vec3 n)  // single directional light
vec3 phongModel ()  // lab 6.3
{

//// diffuse lighting is directional lighting
    vec3 diffuse = vec3(0.0);

//// specular lighting is meant to represent glare upon an object's surface
    vec3 specular = vec3(0.0);

//// ambient lighting is a constant & omnipresent light source
    vec3 ambient = Light.La * Material.Ka;
    // vec3 ambient = lights[light].La * Material.Ka;

    // light direction
    // calculate the direction from the light position to the vertex point with normalize
    vec3 s = normalize(Light.Position.xyz - GPosition.xyz);       // lab 6.3
    // vec3 s = normalize(Light.Position.xyz - position);       // find out s vector
    // vec3 s = normalize(lights[light].Position.xyz - position);  // specialized multiple lights formula

    float cosine = dot( s, GNormal );

    diffuse = Light.Ld * Material.Kd * ceil( cosine * levels ) * scaleFactor;
    /*
    // According to OpenGL SuperBible p.569 (CH13 - Diffuse Light), this dot product caould be a negative value
    // max() is placed to prevent such situation, it will justify the negative part to zero
    float sDotN = max(dot(s, n), 0.0);    // calculate the dot product of normalized normal and light direction

    // diffuseColor = diffuseMaterial * DiffuseLight * the dot product from normal and light direction
    diffuse = Light.Ld * Material.Kd * sDotN;
    // diffuse = lights[light].Ld * Material.Kd * sDotN;

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
        specular = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);        // reflection -> Phong Shading
        // specular = lights[light].Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);   // specialized multiple lights formula

        // specular = Light.Ls * Material.Ks * pow(max(dot(h, n), 0.0), Material.shininess);            // calculate the half vector -> Blinn-Phong Shading
        // specular = lights[light].Ls * Material.Ks * pow(max(dot(h, n), 0.0), Material.shininess);   // specialized multiple lights formula
    
        /// SWITCHABLE ///

    }
    

    // final ouput
    // return diffuse + ambient + specular;
    return diffuse + ambient;   // lab 6.3
}


void main()
{
	if( GIsEdge == 1 )
    {
        FragColor = LineColor;
    }
    else
    {
        FragColor = vec4( phongModel(), 1.0 );
    }
}

*/  // no need in lab 6.4

float edgeMix()
{
    // Find the smallest distance
    float d = min( min( EdgeDistance.x, EdgeDistance.y ), EdgeDistance.z );

    if( d < LineWidth - 1 )
    {
        return 1.0;
    }
    else if( d > LineWidth + 1 )
    {
        return 0.0;
    }
    else
    {
        float x = d - ( LineWidth - 1 );
        return exp2( -2.0 * (x * x) );
    }
}

void main()
{
    // FragColor = LineColor;   

    float mixVal = edgeMix();
    
    FragColor = mix( QuadColor, LineColor, mixVal );
}