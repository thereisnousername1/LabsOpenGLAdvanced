#version 460

// declare the primitives for the geometry shader
// we don't change the geometry of the triangle just pass the data through
// to the fragment shader

layout( triangles ) in;

layout( triangle_strip, max_vertices = 3 ) out;

// Normal sent from geometry shader(to fragment shader)
out vec3 GNormal;	// normal used by fragment shader

// Position sent from geometry shader(to fragment shader)
out vec3 GPosition;	// position used by fragment shader

noperspective out vec3 GEdgeDistance;	// the distance from the edge

in vec3 VNormal[];		// from vertex shader

in vec3 VPosition[];	// from vertex shader

uniform mat4 ViewportMatrix;	// Viewport matrix

void main()
{
	// Transform each vertex into viewport space from clip coordinates
	// divide by the w coordinate as the clip coordinates are homogeneous
	// and need to be converted back to true Cartesian coordinates
	vec2 p0 = vec2( ViewportMatrix * (gl_in[0].gl_Position / gl_in[0].gl_Position.w) );
	vec2 p1 = vec2( ViewportMatrix * (gl_in[1].gl_Position / gl_in[1].gl_Position.w) );
	vec2 p2 = vec2( ViewportMatrix * (gl_in[2].gl_Position / gl_in[2].gl_Position.w) );

	// calculate the edges of the triangle
	float a = length(p1 - p2);
	float b = length(p2 - p0);
	float c = length(p1 - p0);

	// calculate the angles
	float alpha = acos( (b * b + c * c - a * a) / (2.0 * b * c) );
	float beta = acos( (a * a + c * c - b * b) / (2.0 * a * c) );

	// calculate the distances
	float ha = abs( c * sin(beta) );
	float hb = abs( c * sin(alpha) );
	float hc = abs( b * sin(alpha) );

	// pass the distance and position of the new vertex to the fragment shader
	// also pass the normal, position of the mesh

	// setup vertex 1
	GEdgeDistance = vec3( ha, 0, 0 );
	GNormal = VNormal[0];
	GPosition = VPosition[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();	// pass the vertex information to the next stage

	// setup vertex 2
	GEdgeDistance = vec3( 0, hb, 0 );
	GNormal = VNormal[1];
	GPosition = VPosition[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();	// pass the vertex information to the next stage

	// setup vertex 3
	GEdgeDistance = vec3( 0, 0, hc );
	GNormal = VNormal[2];
	GPosition = VPosition[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();	// pass the vertex information to the next stage

	EndPrimitive(); // used to finalise the primitive and send it along the pipeline
}