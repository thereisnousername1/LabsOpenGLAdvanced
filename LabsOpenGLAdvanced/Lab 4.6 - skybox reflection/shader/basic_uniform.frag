#version 460

// this shader is made for lab 4.6

///
/// In fragment shader
/// for how many variables are displayed as in 
/// same variables should come out from vertex shader
///

// intended to be handled by program, less likely needed to be modified
in vec3 ReflectDir;

//in vec2 TexCoord; // only necessary for the skybox, in here reflection is not needed

layout (binding = 0) uniform samplerCube Tex1;    // in the lab it is renamed

// lab 4.6

uniform float ReflectFactor;

uniform vec4 MaterialColor; // this is the simple logic used in the lab video   /// either one

// lab 4.6

// Final Output
layout (location = 0) out vec4 FragColor;

uniform struct MaterialInfo // this is taken from phong model labs              /// would do the work
{
    vec3 Kd;    // diffuse material reflected color
    vec3 Ka;    // ambient material reflected color
    vec3 Ks;    // specular material reflected color

    float shininess;

}Material;

void main()
{
    // lab 4.5

    // vec3 texColor = texture(skyBoxTex, normalize(Vec)).rgb;
    // vec3 texColor = texture(Tex1, normalize(position)).rgb;
    vec3 texColor = texture(Tex1, ReflectDir).rgb;

    texColor = pow(texColor, vec3(1.0 / 2.2));

    // Basic output logic
    // FragColor = vec4(phongModel(position, normalize(normal)), 1.0);
    // FragColor = vec4(texColor, 1.0);

    // Output with reflection, lab solution
    FragColor = vec4(mix(MaterialColor.rgb, texColor, ReflectFactor), 1.0);
    // FragColor = vec4(mix(MaterialColor.rgb, texColor, 2.0), 1.0);
    //                                                   0.0 = no reflect
    //                                                   0.01-0.1 = it looks like ceramic
    
    // Output with reflection, but I don't know why it seems a bit different from lab solution
    //                      shouldn't take ambient inside           same as ReflectFactor
    // FragColor = vec4(mix(vec3(Material.Kd + Material.Ks), texColor, Material.shininess), 1.0);
    // FragColor = vec4(mix(vec4(vec3(Material.Kd + Material.Ks), Material.shininess).xyz, texColor, ReflectFactor), 1.0);
    
    // lab 4.5
}
