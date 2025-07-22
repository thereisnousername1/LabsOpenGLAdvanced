#version 460

// since lab 7 only vertex shader and fragment shader is being used

in float Transp;
in vec2 TexCoord;

uniform sampler2D ParticleTex;

layout ( location = 0 ) out vec4 FragColor;

void main()
{
	FragColor = texture(ParticleTex, TexCoord);
	// Mix with black as it gets older, to simulate a bit of smoke
	FragColor = vec4( mix( vec3(0, 0, 0), FragColor.xyz, Transp ), FragColor.a );
	FragColor.a *= Transp;
}