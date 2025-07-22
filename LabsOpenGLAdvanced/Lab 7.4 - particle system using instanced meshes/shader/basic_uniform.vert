// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

// since lab 7 only vertex shader and fragment shader is being used

const float PI = 3.14159265359;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal; 

layout (location = 3) in vec3 ParticlePosition;
layout (location = 4) in vec3 ParticleVelocity; // former layout (location = 1) in vec3 VertexVelocity;
layout (location = 5) in float ParticleAge;		// former layout (location = 2) in float VertexAge;
layout (location = 6) in vec2 ParticleRotation;

// Render Pass
uniform int Pass;

// Output to transform feedback
//	 (Layout specifiers only available in OpenGL 4.4+)
/*layout( xfb_buffer = 0, xfb_offset = 0 )*/ out vec3 Position;
/*layout( xfb_buffer = 1, xfb_offset = 0 )*/ out vec3 Velocity;
/*layout( xfb_buffer = 2, xfb_offset = 0 )*/ out float Age;
/*layout( xfb_buffer = 3, xfb_offset = 0 )*/ out vec2 Rotation;

// Out to fragment shader

out vec3 fPosition;		// was out vec4 Position;
out vec3 fNormal;		// was out vec3 Normal;
//out float Transp;		// Transparency of the particle, disabled after lab 7.3
//out vec2 TexCoord;	// Texture coordinate, disabled after lab 7.3

uniform float Time;		// Simulation/animation time
uniform float DeltaT;	// Elapsed time between frames
uniform vec3 Accel;		// Particle acceleration (gravity)

// uniform vec3 Gravity = vec3(0.0, -0.05, 0.0);	// Gravity in world coords, disabled since lab 7.3
uniform float ParticleLifetime;		// Max particle lifetime particleLifetime
// uniform float ParticleSize;			// Particle size
uniform vec3 EmitterPos = vec3(0);	// Emitter position in world coordinates, it is Emitter in lab 7.3 and 7.4
uniform mat3 EmitterBasis;			// Rotation that rotates y axis to the direction of emitter

// Transformation matrices
uniform mat4 ModelViewMatrix;	// View * Model
// uniform mat3 NormalMatrix;
// uniform mat4 MVP;

uniform mat4 ProjectionMatrix;

uniform sampler1D RandomTex;	// added in lab 7.3

/*
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
disabled since lab 7.4*/

vec3 randomInitialVelocity()
{
	/* former randomInitialVelocity() logic
	float theta = mix( 0.0, PI / 8.0, texelFetch(RandomTex, 3 * gl_VertexID, 0).r );
	float phi = mix( 0.0, 2.0 * PI, texelFetch(RandomTex, 3 * gl_VertexID + 1, 0).r );
	float velocity = mix( 1.25, 1.5, texelFetch(RandomTex, 3 * gl_VertexID + 2, 0).r );

	vec3 v = vec3( sin(theta) * cos(phi),
				   cos(theta),
				   sin(theta) * sin(phi) );

	return normalize( EmitterBasis * v ) * velocity;
	*/

	float theta = mix( 0.0, PI / 6.0, texelFetch(RandomTex, 4 * gl_VertexID, 0).r );
	float phi = mix( 0.0, 2.0 * PI, texelFetch(RandomTex, 4 * gl_VertexID + 1, 0).r );
	float velocity = mix( 1.25, 1.5, texelFetch(RandomTex, 4 * gl_VertexID + 2, 0).r );

	vec3 v = vec3( sin(theta) * cos(phi),
				   cos(theta),
				   sin(theta) * sin(phi) );

	return normalize( EmitterBasis * v ) * velocity;
}

float randomInitialRotationalVelocity()
{
	return mix( -15.0, 15.0, texelFetch(RandomTex, 4 * gl_VertexID + 3, 0).r );
}

// the update pass where the particles' position, age, and velocity are updated
void update()
{
	/* former void update() logic
	if ( VertexAge < 0 || VertexAge > ParticleLifetime )
	{	// the particle is past it's lifetime, recycle.
		Position = EmitterPos;
		Velocity = randomInitialVelocity();

		if ( VertexAge < 0 )
			Age = VertexAge + DeltaT;
		else
			Age = (VertexAge - ParticleLifetime) + DeltaT;
	}
	else
	{	// the particle is alive, update.
		Position = VertexPosition + VertexVelocity * DeltaT;
		Velocity = VertexVelocity + Accel * DeltaT;

		Age = VertexAge + DeltaT;
	}
	*/

	if ( ParticleAge < 0 || ParticleAge > ParticleLifetime )
	{	// the particle is past it's lifetime, recycle.
		Position = EmitterPos;
		Velocity = randomInitialVelocity();
		Rotation = vec2( 0.0, randomInitialRotationalVelocity() );

		if ( ParticleAge < 0 )
			Age = ParticleAge + DeltaT;
		else
			Age = (ParticleAge - ParticleLifetime) + DeltaT;
	}
	else
	{	// the particle is alive, update.
		Position = ParticlePosition + ParticleVelocity * DeltaT;
		Velocity = ParticleVelocity + Accel * DeltaT;
		Rotation.x = mod( ParticleRotation.x + ParticleRotation.y * DeltaT, 2.0 * PI );
		Rotation.y = ParticleRotation.y;

		Age = ParticleAge + DeltaT;
	}
}

// the render pass where the particles are drawn
void render()
{
	float cs = cos(ParticleRotation.x);
	float sn = sin(ParticleRotation.x);

	mat4 rotationAndTranslation = mat4( 1, 0, 0, 0,
										0, cs, sn, 0, 
										0, -sn, cs, 0, 
										ParticlePosition.x, ParticlePosition.y, ParticlePosition.z, 1 );

	mat4 m = ModelViewMatrix * rotationAndTranslation;

	fPosition = ( m * vec4(VertexPosition, 1) ).xyz;
	fNormal = ( m * vec4(VertexNormal, 0) ).xyz;

	// Draw at the current position
	gl_Position = ProjectionMatrix * vec4(fPosition, 1.0);
}

void main()
{
	if ( Pass == 1 )
		update();
	else
		render();
}