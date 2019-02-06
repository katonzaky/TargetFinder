
#version 330

// Just interpolate the lit colour computed for each vertex

in float flagS;
in vec4 litColour;
in vec4 trans;
in vec3 p;
in vec4 uv;
in vec4 uv_diffuse;
in vec2 st;
in float flagL;

uniform sampler2D texMap;

out vec4 fragColour;

void main()
{	

	//fragColour = litColour * texture(texMap, st);
	
	if (flagS == 2)
	{
		fragColour = uv;
	}
	else if (flagS == 3)
	{
		fragColour = uv_diffuse;
	}
	else if (flagS == 0 || flagS == 1)
	{
		fragColour = litColour * texture(texMap, st);
	}

	if (flagL == 4)
	{
		fragColour = vec4(1.0, 1.0, 1.0, 1.0) * texture(texMap, st);
	}
	else
	{
		fragColour = litColour * texture(texMap, st);
	}
}
