#version 330

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;

out vec3 Normal;
out vec3 Position;

float gl_ClipDistance[1];

uniform mat4 modelview_matrix;
uniform mat4 view;
uniform mat4 projection_matrix;

void main()
{
    Normal = mat3(transpose(inverse(modelview_matrix))) * a_normal;
    Position = vec3(modelview_matrix * vec4(a_vertex, 1.0));
    gl_Position = projection_matrix * view * modelview_matrix * vec4(a_vertex, 1.0);
    
	//gl_ClipDistance[0] = dot(modelview_matrix * a_vertex, Plane);
}  
