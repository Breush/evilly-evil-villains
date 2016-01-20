uniform sampler2D texture;
uniform vec2 position;
uniform vec2 size;

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    vec2 halfSize = size / 2.f;
    vec2 center = position + vec2(halfSize.x, halfSize.y);
    vec2 distCenter = gl_FragCoord.xy - center;

    // Blind some
    float x = (distCenter.x * distCenter.x) / (halfSize.x * halfSize.x);
    float y = (distCenter.y * distCenter.y) / (halfSize.y * halfSize.y);

    float norm = 0.f;
    if (x < 0.95f && y < 0.95f)
        norm = (0.95f - x) * (0.95f - y) / 0.9025f;

    gl_FragColor = gl_Color * pixel * norm;
}

