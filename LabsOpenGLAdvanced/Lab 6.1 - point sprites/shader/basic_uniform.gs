#version 460

layout( points ) in;	// we define the type of primitives we receive - points in this case

layout( triangle_strip, max_vertices = 4 ) out;	// the type of primitives we produce - triangle
												// strips with the maximum number of vertices = 4

uniform float Size2;	// Half the width of the quad

uniform mat4 ProjectionMatrix; // for transformation into the clip space

out vec2 TexCoord;	// used in our fragment shader

void main()
{
	mat4 m = ProjectionMatrix; // assign the projection matrix

	// setup vertex 1
	gl_Position = m * ( vec4(-Size2, -Size2, 0.0, 0.0) + gl_in[0].gl_Position );	// we access the information through gl_in

	TexCoord = vec2(0.0, 0.0); // update texture coordinates
	EmitVertex();	// pass the vertex information to the next stage

	// setup vertex 2
	gl_Position = m * ( vec4(Size2, -Size2, 0.0, 0.0) + gl_in[0].gl_Position );
	TexCoord = vec2(1.0, 0.0);
	EmitVertex();

	// setup vertex 3
	gl_Position = m * ( vec4(-Size2, Size2, 0.0, 0.0) + gl_in[0].gl_Position );
	TexCoord = vec2(0.0, 1.0);
	EmitVertex();

	// setup vertex 4
	gl_Position = m * ( vec4(Size2, Size2, 0.0, 0.0) + gl_in[0].gl_Position );
	TexCoord = vec2(1.0, 1.0);
	EmitVertex();

	EndPrimitive(); // used to finalise the primitive and send it along the pipeline
}