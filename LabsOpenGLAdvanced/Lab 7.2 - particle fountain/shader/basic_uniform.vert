// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

// since lab 7 only vertex shader and fragment shader is being used

layout (location = 0) in vec3 VertexInitVel;		// Particle initial velocity
layout (location = 1) in float VertexBirthTime;		// Particle birth time

//out vec4 Position;	// disabled since lab 7.2
//out vec3 Normal;		// disabled since lab 7.2

out float Transp;	// Transparency of the particle
out vec2 TexCoord;	// Texture coordinate

uniform float Time; // animation time

uniform vec3 Gravity = vec3(0.0, -0.05, 0.0);	// Gravity in world coords
uniform float ParticleLifetime;					// Max particle lifetime particleLifetime
uniform float ParticleSize = 1.0;				// Particle size
uniform vec3 EmitterPos;						// Emitter position in world coordinates

// Transformation matrices
uniform mat4 ModelViewMatrix;
// uniform mat3 NormalMatrix;
// uniform mat4 MVP;

uniform mat4 ProjectionMatrix;

// Offsets to the position in camera coordinates for each vertex of the particle's quad
const vec3 offsets[] = vec3[]( vec3(-0.5, -0.5, 0),
							   vec3(0.5, -0.5, 0),
							   vec3(0.5, 0.5, 0),
							   vec3(-0.5, -0.5, 0),
							   vec3(0.5, 0.5, 0),
							   vec3(-0.5, 0.5, 0) );

// Texture coordinates for each vertex of the particle's quads
const vec2 texCoords[] = vec2[]( vec2(0, 0),
								 vec2(1, 0),
								 vec2(1, 1),
								 vec2(0, 0),
								 vec2(1, 1),
								 vec2(0, 1) );

void main()
{
	vec3 cameraPos;		// Position in camera coordinates
	float t = Time - VertexBirthTime;
	if( t >= 0 && t < ParticleLifetime )
	{
		vec3 pos = EmitterPos + VertexInitVel * t + Gravity * t * t;
		// offset the vertex based on the ID
		cameraPos = ( ModelViewMatrix * vec4(pos, 1) ).xyz + ( offsets[gl_VertexID] * ParticleSize );
		Transp = mix( 1, 0, t / ParticleLifetime );
	}
	else
	{
		// Particle doesn't "exist", draw fully transparent
		cameraPos = vec3(0);
		Transp = 0.0;
	}

	TexCoord = texCoords[gl_VertexID];

	gl_Position = ProjectionMatrix * vec4(cameraPos, 1);
}