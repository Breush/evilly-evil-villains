uniform sampler2D texture;
uniform vec2 textureSize;
uniform vec2 screenSize;
uniform vec2 effectiveDisplay;
uniform vec2 resolution;
uniform vec2 position;

void main()
{
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
	float norm = 0.f;

	vec2 halfTexture = textureSize / 2.f;
    vec2 halfGap = (screenSize - effectiveDisplay) / 2.f;
    vec2 viewRatio = resolution / effectiveDisplay;

    // Correct position (SFML <-> GLSL)
    vec2 fragNormalized = (gl_FragCoord.xy - halfGap) * viewRatio;
    vec2 glslPosition = vec2(position.x, resolution.y - position.y);
    vec2 center = fragNormalized - (glslPosition + vec2(halfTexture.x, -halfTexture.y));

    // Blind some
	float x = (center.x * center.x) / (halfTexture.x * halfTexture.x);
	float y = (center.y * center.y) / (halfTexture.y * halfTexture.y);

    if (x < 0.95f && y < 0.95f)
    	norm = (0.95f - x) * (0.95f - y) / 0.9025f;

    gl_FragColor = gl_Color * pixel * norm;
}

