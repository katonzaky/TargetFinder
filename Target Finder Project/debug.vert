
// Calculates colour of each vertex using Phong shading.
// Uses ambient, diffuse and specular terms.

#version 330

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoords;
layout (location = 3) in vec3 a_tangents;
layout (location = 4) in vec3 a_bitangents;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    
	vec3 SpotTangentLightPos;
    vec3 PointTangentLightPos;
    vec3 DirTangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform vec3 light1_pos;
uniform vec3 light1_dir;
uniform vec3 light2_pos;
uniform vec3 light3_pos;
uniform vec3 light3_dir;
uniform vec3 viewPos;

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;
uniform mat4 view;
uniform mat4 lightSpaceMatrix;

void main(void)
{
    vs_out.FragPos = vec3(modelview_matrix * vec4(a_vertex, 1.0));
    vs_out.Normal = transpose(inverse(mat3(modelview_matrix))) * a_normal;
    vs_out.TexCoords = a_texcoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(modelview_matrix)));
    vec3 T = normalize(normalMatrix * a_tangents);
    vec3 N = normalize(normalMatrix * a_normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N)); 
    vs_out.DirTangentLightPos = TBN * light1_pos;
    vs_out.PointTangentLightPos = TBN * light2_pos;
    vs_out.SpotTangentLightPos = TBN * light3_pos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vec3(modelview_matrix * vec4(a_vertex, 0.0));


    gl_Position = projection_matrix * view * modelview_matrix * vec4(a_vertex, 1.0f);
}

