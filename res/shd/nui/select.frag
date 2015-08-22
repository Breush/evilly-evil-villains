uniform sampler2D texture;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    vec4 original = texture2D(texture, texCoord);
    vec4 pixel = vec4(0.f);

    // Having range bigger to get a better effect
    // can slow the computer down
    const float step = 0.005f;
    const int range = 3;

    // Blur
    for (int x = -range; x <= range; ++x)
    for (int y = -range; y <= range; ++y)
        pixel += texture2D(texture, texCoord + vec2(x, y) * step);

    // Change original colors - to something a bit more blue
    vec4 colorModifier = vec4(0.9f, 0.95f, 1.f, 1.f);

    // Adding original gives a bloom effect
    // by reinforcing the original colors
    const float additions = float(2 * range + 1) * float(2 * range + 1);
    gl_FragColor = gl_Color * colorModifier * (original + pixel / additions);
}
