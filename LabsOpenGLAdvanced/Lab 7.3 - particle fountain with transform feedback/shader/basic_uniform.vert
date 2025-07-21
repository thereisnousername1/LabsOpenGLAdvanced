// install a GLSL tool to edit any shader more easily
// for they are written in GLSL language
#version 460

// since lab 7 only vertex shader and fragment shader is being used

const float PI = 3.14159265359;

layout (location = 0) in vec3 VertexPosition;	// former vec3 VertexInitVel;		// Particle initial velocity
layout (location = 1) in vec3 VertexVelocity;	// former float VertexBirthTime;		// Particle birth time
layout (location = 2) in float VertexAge;

// Render Pass
uniform int Pass;

// Output to transform feedback buffers update pass
//	 (Layout specifiers only available in OpenGL 4.4+)
/*layout( xfb_buffer = 0, xfb_offset = 0 )*/ out vec3 Position;
/*layout( xfb_buffer = 1, xfb_offset = 0 )*/ out vec3 Velocity;
/*layout( xfb_buffer = 2, xfb_offset = 0 )*/ out float Age;

// Out to fragment shader

//out vec4 Position;	// disabled since lab 7.2
//out vec3 Normal;		// disabled since lab 7.2
out float Transp;	// Transparency of the particle
out vec2 TexCoord;	// Texture coordinate

uniform float Time;		// Simulation/animation time
uniform float DeltaT;	// Elapsed time between frames
uniform vec3 Accel;		// Particle acceleration (gravity)

// uniform vec3 Gravity = vec3(0.0, -0.05, 0.0);	// Gravity in world coords, disabled since lab 7.3
uniform float ParticleLifetime;		// Max particle lifetime particleLifetime
uniform float ParticleSize;			// Particle size
uniform vec3 EmitterPos = vec3(0);	// Emitter position in world coordinates, it is Emitter in lab 7.3
uniform mat3 EmitterBasis;			// Rotation that rotates y axis to the direction of emitter

// Transformation matrices
uniform mat4 ModelViewMatrix;	// View * Model
// uniform mat3 NormalMatrix;
// uniform mat4 MVP;

uniform mat4 ProjectionMatrix;

uniform sampler1D RandomTex;	// added in lab 7.3

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

vec3 randomInitialVelocity()
{
	/* former initBuffers() in scenebasic_uniform.cpp, at least same logic
	// Fill the first velocity buffer with random velocities
    glm::mat3 emitterBasis = ParticleUtils::makeArbitraryBasis(emitterDir);
    vec3 v(0.0f);
    float velocity, theta, phi;
    std::vector<GLfloat> data(nParticles * 3);
    for (uint32_t i = 0; i < nParticles; i++)
    {
        // Pick the direction of the velocity
        theta = glm::mix(0.0f, glm::pi<float>() / 20.0f, randFloat());
        phi = glm::mix(0.0f, glm::two_pi<float>(), randFloat());

        v.x = sinf(theta) * cosf(phi);
        v.y = cosf(theta);
        v.z = sinf(theta) * sinf(phi);

        // scale to set the magnitude of the velocity
        velocity = glm::mix(1.25f, 1.5f, randFloat());
        v = glm::normalize(emitterBasis * v) * velocity;

        data[3 * i] = v.x;
        data[3 * i + 1] = v.y;
        data[3 * i + 2] = v.z;
    }
	*/

	float theta = mix( 0.0, PI / 8.0, texelFetch(RandomTex, 3 * gl_VertexID, 0).r );
	float phi = mix( 0.0, 2.0 * PI, texelFetch(RandomTex, 3 * gl_VertexID + 1, 0).r );
	float velocity = mix( 1.25, 1.5, texelFetch(RandomTex, 3 * gl_VertexID + 2, 0).r );

	vec3 v = vec3( sin(theta) * cos(phi),
				   cos(theta),
				   sin(theta) * sin(phi) );

	return normalize( EmitterBasis * v ) * velocity;
}

// the update pass where the particles' position, age, and velocity are updated
void update()
{
	/* former void main() logic
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
	*/

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
}

// the render pass where the particles are drawn
void render()
{
	/* partially former camera logic in void main()
	vec3 cameraPos;		// Position in camera coordinates
	
	...

	TexCoord = texCoords[gl_VertexID];

	gl_Position = ProjectionMatrix * vec4(cameraPos, 1);
	*/

	Transp = 0.0;
	vec3 cameraPos = vec3(0.0);	// it is called posCam in lab 7 lab sheet p.13

	if ( VertexAge >= 0.0 )
	{
		// former cameraPos = ModelViewMatrix * vec4(pos, 1) ).xyz + ( offsets[gl_VertexID] * ParticleSize );
		cameraPos = ( ModelViewMatrix * vec4(VertexPosition, 1) ).xyz + offsets[gl_VertexID] * ParticleSize;

		// former Transp = mix( 1, 0, t / ParticleLifetime );
		Transp = clamp( 1.0 - VertexAge / ParticleLifetime, 0, 1 );
	}

	TexCoord = texCoords[gl_VertexID];

	gl_Position = ProjectionMatrix * vec4(cameraPos, 1);
}

void main()
{
	if ( Pass == 1 )
		update();
	else
		render();
}