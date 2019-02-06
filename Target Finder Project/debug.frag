#version 330

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    
    vec3 SpotTangentLightPos;
    vec3 PointTangentLightPos;
    vec3 DirTangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D texMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;
uniform sampler2D displaceMap;
uniform samplerCube pointShadowMap;

uniform vec3 light1_dir;
uniform vec3 light1_pos;
uniform vec3 light2_pos;
uniform vec3 light3_pos;
uniform vec3 light3_dir;
uniform vec3 viewPos;


uniform float flashlight;
uniform float displaceFlag;
uniform float bumpFlag;
uniform float far_plane;
uniform float mtl_shininess;
uniform vec3 mtl_ambient;  // Ambient surface colour
uniform vec3 mtl_diffuse;  // Diffuse surface colour
uniform vec3 mtl_specular; // Specular surface colour

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), 
   vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
   vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
   vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    vec3 lightDir = normalize(light1_pos - fs_in.FragPos);
    float bias =  max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 20.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float SpotShadowCalculation(vec4 fragPosLightSpace, vec3 normal)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    vec3 lightDir = normalize(light3_dir - fs_in.FragPos);
    float bias =  max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 20.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float PointShadowCalculation(vec3 fragPos)
{
    // Get vector between fragment position and light position
    vec3 fragToLight = fragPos - light2_pos;
    // Get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // Test for shadows with PCF
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // Undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // Display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    // return shadow;
    return shadow;
}  

// Calculates the color when using a directional light.
vec4 CalcDirLight(vec4 color, vec3 normal, mat3 TBN)
{
    vec3 light_ambient = vec3(0.1f, 0.1f, 0.1f);
    vec3 light_diffuse = vec3(1.0f, 0.7f, 0.1f);
    vec3 light_specular = vec3(0.5f, 0.5f, 0.5f);
    // Ambient
    vec3 ambient = light_ambient * mtl_ambient;
    // Diffuse
    vec3 lightDir = normalize((TBN * light1_pos));

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light_diffuse * mtl_diffuse;

    // Specular
    vec3 viewDir = normalize((TBN * viewPos) - (TBN * fs_in.FragPos));
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), mtl_shininess);
    vec3 specular = light_specular * spec * mtl_specular;    
    // Calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal);                     
    shadow = min(shadow, 1.0f); // reduce shadow strength a little: allow some diffuse/specular light in shadowed regions

    return vec4((ambient + (1.0 - shadow) * (diffuse + specular)), 1.0f) * color;  
}

// Calculates the color when using a directional light.

vec4 CalcSpotLight(vec4 color, vec3 normal)
{
    vec3 light_ambient = vec3(0.12f, 0.12f, 0.12f);
    vec3 light_diffuse = vec3(1.0f, 1.0f, 1.0f);
    vec3 light_specular = vec3(0.5f, 0.7f, 0.7f);
    float cutOff = radians(45.0f);
    vec3 lightDir = normalize(light3_pos - fs_in.FragPos);
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    // check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light3_dir)); 
    
    if(theta > cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
    {    
        // ambient
        vec3 ambient = light_ambient * color.rgb;
        
        // diffuse 
        vec3 norm = normalize(fs_in.Normal);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light_diffuse * diff * color.rgb;  
        
        // specular
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), mtl_shininess);
        vec3 specular = light_specular * spec * color.rgb;  
        
        // attenuation
        float distance    = length(light3_pos - fs_in.FragPos);
        float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));    

        // ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
        diffuse   *= attenuation;
        specular *= attenuation;   
            
		float shadow = SpotShadowCalculation(fs_in.FragPosLightSpace, normal);                     
		shadow = min(shadow, 1.0f); // reduce shadow strength a little: allow some diffuse/specular light in shadowed regions

        vec4 result = vec4((ambient + (1.0 - shadow) * (diffuse + specular)), 1.0f) * color;  
        return result;
    }
    else 
    {
        // else, use ambient light so scene isn't completely dark outside the spotlight.
        //FragColor = vec4(light.ambient * texture(material.diffuse, TexCoords).rgb, 1.0);
        return vec4(0.0f);
    } 
}

// Calculates the color when using a point light.
vec4 CalcPointLight(vec4 color, vec3 normal, mat3 TBN)
{
    vec3 light_ambient = vec3(0.12f, 0.12f, 0.12f);
    vec3 light_diffuse = vec3(1.0f, 1.0f, 1.0f);
    vec3 light_specular = vec3(0.5f, 0.5f, 0.5f);

    // Ambient
    vec3 ambient = light_ambient * mtl_ambient;
    // Diffuse
    vec3 lightDir = normalize((TBN * light2_pos) - (TBN * fs_in.FragPos));
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light_diffuse * mtl_diffuse;
    // Specular
    vec3 viewDir = normalize((TBN * viewPos) - (TBN * fs_in.FragPos));
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), mtl_shininess);
    vec3 specular = light_specular * spec * mtl_specular;    
    // Calculate shadow
    float shadow = PointShadowCalculation(fs_in.FragPos);                      
    shadow = min(shadow, 1.0f); // reduce shadow strength a little: allow some diffuse/specular light in shadowed regions

    return vec4((ambient + (1.0 - shadow) * (diffuse + specular)), 1.0f) * color;    
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
	float heightScale = 0.1f;
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(displaceMap, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(displaceMap, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(displaceMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;    
} 

void main()
{
    //gamma correction
    float gamma = 0.5f;

	vec3 viewDir;
	vec2 texCoords;
    vec3 normal;
    const float density = 0.03;
    const float LOG2 = 1.442695;
    float z = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = exp2(-density * density * z * z * LOG2);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec4 fog_color = vec4(0.9,0.9,1.0,0);
    
    vec3 p_dx = dFdx(fs_in.FragPos);
    vec3 p_dy = dFdy(fs_in.FragPos);
    // compute derivations of the texture coordinate
    vec2 tc_dx = dFdx(fs_in.TexCoords);
    vec2 tc_dy = dFdy(fs_in.TexCoords);
    // compute initial tangent and bi-tangent
    vec3 t = normalize( tc_dy.y * p_dx - tc_dx.y * p_dy );
    vec3 b = normalize( tc_dy.x * p_dx - tc_dx.x * p_dy ); // sign inversion
    // get new tangent from a given mesh normal
    vec3 n = normalize(fs_in.Normal);
    vec3 x = cross(n, t);
    t = cross(x, n);
    t = normalize(t);
    // get updated bi-tangent
    x = cross(b, n);
    b = cross(n, x);
    b = normalize(b);

    mat3 TBN = mat3(t, b, n); 

    if (bumpFlag == 1.0)
    {
        // offset texture coordinates with Parallax Mapping
        vec3 viewDir = normalize((TBN * viewPos) - (TBN * fs_in.FragPos));
        texCoords = fs_in.TexCoords;
        
        if (displaceFlag == 1.0f)
        	texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);       
        //if(texCoords.x > 25.0 || texCoords.y > 25.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
            //discard;

        // obtain normal from normal map
        normal = texture(normalMap, texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);   
        //normal = normalize(fs_in.Normal);     
    }
    else
    {
    	texCoords = fs_in.TexCoords;
    	normal = normalize(fs_in.Normal);
    }
    
    vec4 color = pow(texture(texMap, texCoords), vec4(gamma));

   	vec4 lighting = CalcDirLight(color, normal, TBN);
    lighting += CalcPointLight(color, normal, TBN);
    
    if (flashlight == 1.0f)
    	lighting += CalcSpotLight(color, normal);   

    FragColor = lighting;
    FragColor = pow(mix(fog_color, FragColor, fogFactor), vec4(1.0/gamma));
}
