#version 120

// Input textures
uniform sampler2D normalsTexture;
uniform sampler2D lightTexture;
uniform vec2 targetSize;
uniform vec2 lightSize;

// Light properties
uniform vec3 lightPosition;
uniform vec3 lightColor;

void main()
{
	// Sample our normals map
	vec2 coord = gl_TexCoord[0].xy;
	float lightPower = texture2D(lightTexture, coord).r;
	vec4 normalsColor = texture2D(normalsTexture, gl_FragCoord.xy / targetSize);
	
	// Get normal value from sample
	vec3 normals = normalize(normalsColor.rgb * 2.0 - 1.0);
	
	// Compute the light vector
	vec2 lightVector = lightPosition.xy - gl_FragCoord.xy;
	vec3 lightDir = vec3(lightVector / lightSize, lightPosition.z);
	lightDir = normalize(lightDir);
	
	// Compute the color intensity based on normals
	float colorIntensity = max(dot(normals, lightDir), 0.0);
	vec4 diffuse = vec4(lightColor * colorIntensity, lightPower);
	
	gl_FragColor = clamp(diffuse, 0.0, 1.0);
}

