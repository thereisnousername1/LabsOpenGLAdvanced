#version 460

///
/// From lab 6 the main focus switch to geometry shader
/// fragment shader will be simplify and phongmodel(or any other parts in previous lab)
/// will be added only when it's necessary for to use or commenting
/// Improvement of readability
///

// lab 6.2

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

// line struct for rendering the wireframe
uniform struct LineInfo
{
    float Width;
    vec4 Color;
}Line;

// in variables coming from geometry shader
in vec3 GPosition;
in vec3 GNormal;
noperspective in vec3 GEdgeDistance;


// lab 6.2

// in vec2 TexCoord;            // disabled since lab 6.2

// uniform sampler2D SpriteTex;	// disabled since lab 6.2

// out color
layout(location = 0) out vec4 FragColor;

// in the video Blinn-Phong is used, but in my study I want to keep both

// vec3 phongModel (int light, vec3 position, vec3 n)   // multiple directional lights
vec3 phongModel (vec3 position, vec3 n)  // single directional light
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
    vec3 s = normalize(Light.Position.xyz - position);       // find out s vector
    // vec3 s = normalize(lights[light].Position.xyz - position);  // specialized multiple lights formula

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
    return diffuse + ambient + specular;
}

void main()
{
	//					Sprite Texture according to the Texture coordinate, I think
	// FragColor = texture(SpriteTex, TexCoord);

    // calculate the color
    vec4 color = vec4( phongModel(GPosition, GNormal), 1.0 );
    // vec4 color = vec4( pow(phongModel(GPosition, GNormal), vec3( 1.0/2.2 )), 1.0 ); // added with gamma logic

    // Find the smallest distance for the fragment
    float d = min( GEdgeDistance.x, GEdgeDistance.y );

    d = min( d, GEdgeDistance.z );

    float mixVal;
    if( d < Line.Width - 1 )
    {
        mixVal = 1.0;
    }
    else if( d > Line.Width + 1 )
    {
        mixVal = 0.0;
    }
    else
    {
        float x = d - ( Line.Width - 1 );
        mixVal = exp2( -2.0 * (x * x) );
    }

    FragColor = mix( color, Line.Color, mixVal );
}