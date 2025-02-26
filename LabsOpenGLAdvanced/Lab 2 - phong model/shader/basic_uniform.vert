// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

layout (location = 0) in vec3 VertexPosition;

// lab 2

// layout (location = 1) in vec3 VertexColor;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

// lab 2

// out vec3 Color; // useless since lab 2

// lab 1

// uniform mat4 RotationMatrix; // useless since lab 2

// lab 1

/* lab 2 - diffuse

uniform vec4 LightPosition;

uniform vec3 Kd;

uniform vec3 Ld;

uniform mat4 ModelViewMatrix;

uniform mat4 MVP;

uniform mat3 NormalMatrix;

*/

// lab 2 - phong model

// uniform vec4 LightPosition;  // disabled since lab 2 - phong model
uniform struct LightInfo        // extend to a class
{
    vec4 Position;

    vec3 Ld;    // diffuse light

    vec3 La;    // ambient light

    vec3 Ls;    // specular light

}Light;

uniform struct MaterialInfo        // extend to a class
{
    vec3 Kd;    // diffuse Material

    vec3 Ka;    // ambient Material

    vec3 Ks;    // specular Material

    // how shiny the specular light is
    float shininess;

}Material;

// lab 2 - phong model

uniform mat4 ModelViewMatrix;

uniform mat4 MVP;

uniform mat3 NormalMatrix;

void getCamSpaceValues(out vec3 n, out vec3 pos)
{
    n = normalize(NormalMatrix * VertexNormal); 
    pos = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
}

void main()
{
    // lab 2 - diffuse

    // Color = VertexColor;

    // normalized normal
    // page 5 of lecture 2 notes(COMP3015)
    // convert the vertex normal to eye coordinates
    // vec3 n = normalize(NormalMatrix * VertexNormal);         // replaced since lab 2 - phong model

    // convert the vertex position to eye coordinates
    // vec4 pos = ModelViewMatrix * vec4(VertexPosition, 1.0);  // replaced since lab 2 - phong model

    // lab 2 - phong model
    
    vec3 n, pos;

    getCamSpaceValues(n, pos);

//// ambient lighting is a constant & omnipresent light source
    vec3 ambient = Light.La * Material.Ka;

    // lab 2 - phong model

    // light direction
    // calculate the direction from the light position to the vertex point with normalize
    // vec3 s = normalize(vec3(LightPosition - pos));
    // vec3 s = normalize(vec3(Light.Position - pos));
    vec3 s = normalize(vec3(Light.Position) - pos);

    // According to OpenGL SuperBible p.569 (CH13 - Diffuse Light), this dot product caould be a negative value
    // max() is placed to prevent such situation, it will justify the negative part to zero
    // calculate the dot product of normalized normal and light direction
    float sDotN = max(dot(s, n), 0.0);

    // experiment when we don't use max()
    // float sDotN = dot(s, n);

//// diffuse lighting is directional lighting
    // diffuseColor = diffuseMaterial * DiffuseLight * the dot product from normal and light direction
    // vec3 diffuse = Ld * .Kd * sDotN;
    vec3 diffuse = Light.Ld * Material.Kd * sDotN;

    // lab 2 - phong model

//// specular lighting is meant to represent glare upon an object's surface
    vec3 specular = vec3(0.0);

    if (sDotN > 0.0)
    {
        // p.569 of opengl superbible
        // eye normal
        vec3 v = normalize(-pos.xyz);

        // reflection
        vec3 r = reflect(-s, n);

        //  final spec                           eye reflection angle
        specular = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.shininess);
    }

    // lab 2 - phong model
    // final ouput = diffuse, in the following lab there could be more different light implemented and output with diffuse
    // LightIntensity = diffuse;
    LightIntensity = diffuse + ambient + specular;

    // lab 2 - diffuse

    

    // lab 1
    // gl_Position = vec4(VertexPosition,1.0);  // disabled from lab 1
    // gl_Position = vec4(VertexPosition,1.0) * RotationMatrix; // diabled from lab 2
    gl_Position = MVP * vec4(VertexPosition,1.0);

    
}
