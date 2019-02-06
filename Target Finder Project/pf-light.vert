
// Per-fragment Phong lighting.
// The vertex shader converts vertex position and normal in eye space.
// Passes these to the fragment shader for per-fragment Phong calculation.

#version 330

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;
uniform mat4 view;

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;

out vec4 vertex; // vertex position in eye space
out vec3 normal; // the eye space normal

void main(void) 
{
    vertex = modelview_matrix * vec4(a_vertex, 1.0);
	normal = normalize(normal_matrix * a_normal);
	
	gl_Position = projection_matrix * view * vertex;
}
