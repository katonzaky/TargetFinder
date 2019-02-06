#version 330

out vec4 FragColour;

in vec3 Normal;
in vec3 Position;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{           
	float ratio = 1.00 / 1.33;  
    vec3 I = normalize(Position - viewPos);
    vec3 R1 = refract(I, normalize(Normal), ratio);
    vec3 R2 = reflect(I, normalize(Normal));
    vec3 R = R1 + R2;
    FragColour = vec4(texture(skybox, R).rgb, 1.0);
}
