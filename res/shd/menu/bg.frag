uniform sampler2D texture;
uniform vec2 screenSize;

void main()
{
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
	vec2 halfScreen = screenSize / 2.f;
	vec2 center = gl_FragCoord.xy - halfScreen;
	float norm = 1.f - dot(center, center) / dot(halfScreen, halfScreen);

	gl_FragColor = gl_Color * pixel * norm;
}
