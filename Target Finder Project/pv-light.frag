
#version 330

// Just interpolate the lit colour computed for each vertex

in vec4 litColour;
in vec4 trans;
in vec3 p;

out vec4 fragColour;
#define M_PI 3.141592653589793238462643383279

//http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{	
	float rng1 = rand(vec2(trans.x, trans.y));
 	float x = M_PI*(p.x + 1.0);
	float y = M_PI*(p.y + 1.0);

	if ((mod(10.0 * x, 5.0) < 1.0))
	{
		fragColour = litColour * trans;
	}
	else if (mod(10.0 * y, 5.0) < 1.0)
	{
		fragColour = litColour * vec4(1.0, 1.0, 1.0, 1.0);
	}
	else
	{
		fragColour = litColour;
	}
}
