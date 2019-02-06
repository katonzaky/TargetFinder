
#version 330 

layout (location = 0) in vec3 position;

uniform mat4 projection_matrix;
uniform mat4 view;

out vec3 TexCoords;

void main()
{
    vec4 pos = projection_matrix * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
    TexCoords = position;
}  