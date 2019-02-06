
// Calculates colour of each vertex using Phong shading.
// Uses ambient, diffuse and specular terms.

#version 330

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;
uniform mat4 view;
uniform vec4 light_pos;			// Light position in eye-space

uniform vec3 light_ambient;     // Light ambient RGBA values
uniform vec3 light_diffuse;     // Light diffuse RGBA values  
uniform vec3 light_specular;    // Light specular RGBA values

uniform vec3 mtl_ambient;  // Ambient surface colour
uniform vec3 mtl_diffuse;  // Diffuse surface colour
uniform vec3 mtl_specular; // Specular surface colour

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;

out vec4 litColour;
out vec4 litColour2;
out vec4 pos;
out vec3 p;
out vec4 trans;
// These control how quickly the light falls off based on distance
// Not used in this version of the shader.
const float constantAttn = 0.0;
const float linearAttn = 0.0;
const float quadraticAttn = 0.05;

// The shininess value is an exponent for the specular reflection
// It controls how spread out or sharp the highlight is
// It is the property of the surface material, not the light
// 4 - matte
// 16 - glossy/metallic
// 64 - plastic
// 128 - wet
const float shininess = 128;

// This function computes the Phong lit colour for a vertex
// Parameters are the vertex position and normal in eye space.
vec3 phongPointLight(in vec4 position, in vec3 norm)
{
    // s is the direction from the light to the vertex
    vec3 s = normalize(vec3(light_pos - position));

    // v is the direction from the eye to the vertex
    vec3 v = normalize(-position.xyz);

    // r is the direction of light reflected from the vertex
    vec3 r = reflect( -s, norm );
    
    vec3 ambient = light_ambient * mtl_ambient;
	
    // The diffuse component
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = light_diffuse * mtl_diffuse * sDotN;

    // Specular component
    vec3 spec = vec3(0.0);
    if ( sDotN > 0.0 )
		spec = light_specular * mtl_specular *
            pow( max( dot(r,v), 0.0 ), shininess );

    return ambient + diffuse + spec;
}

vec3 phongPointLight2(in vec4 position, in vec3 norm)
{
    // s is the direction from the light to the vertex
    vec3 s = normalize(vec3(light_pos - position));

    // v is the direction from the eye to the vertex
    vec3 v = normalize(-position.xyz);

    // r is the direction of light reflected from the vertex
    vec3 r = reflect( -s, norm );
    
    vec3 ambient = light_ambient * mtl_ambient;
    
    // The diffuse component
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = vec3(1.0,1.0,1.0) * vec3(1.0,1.0,1.0) * sDotN;

    // Specular component
    vec3 spec = vec3(0.0);
    if ( sDotN > 0.0 )
        spec = light_specular * mtl_specular *
            pow( max( dot(r,v), 0.0 ), shininess );

    return ambient + diffuse + spec;
}

void main(void)
{
    // Convert normal and position to eye coords
    vec3 eyeNorm = normalize( normal_matrix * a_normal);
    vec4 eyePos = modelview_matrix * vec4(a_vertex, 1.0);

    
    litColour = vec4(phongPointLight(eyePos, eyeNorm), 1.0);
    litColour2 = vec4(phongPointLight2(eyePos, eyeNorm), 1.0);
	
    gl_Position = projection_matrix * view * eyePos;
	
	trans = gl_Position;
	p = a_vertex;
}

