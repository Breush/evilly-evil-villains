uniform sampler2D texture;
uniform vec2 screenSize;
uniform vec2 effectiveDisplay;

void main()
{
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    vec2 halfDisplay = effectiveDisplay / 2.f;
	vec2 centered = gl_FragCoord.xy - screenSize / 2.f;
    float norm = 1.f;

    if (abs(centered.x) < halfDisplay.x || abs(centered.y) < halfDisplay.y)
        norm -= dot(centered, centered) / dot(halfDisplay, halfDisplay);

	gl_FragColor = gl_Color * pixel * norm;
}
