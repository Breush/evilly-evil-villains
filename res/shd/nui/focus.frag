uniform sampler2D texture;
uniform vec2 textureSize;
uniform vec2 screenSize;
uniform vec2 position;

void main()
{
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    
    // Correct position
    vec2 realPosition = position;
    realPosition.y = screenSize.y - textureSize.y - position.y;
    vec2 coord = gl_FragCoord.xy - realPosition;
    
    // Blind some
	vec2 halfTexture = textureSize / 2.f;
    vec2 center = coord - halfTexture;
	float x = (center.x * center.x) / (halfTexture.x * halfTexture.x);
	float y = (center.y * center.y) / (halfTexture.y * halfTexture.y);
    
    if (x < 0.9f && y < 0.9f)
    	pixel *= (0.9f - x) * (0.9f - y) / 0.81f;
    else
    	pixel *= 0.f;

    gl_FragColor = gl_Color * pixel;
}

