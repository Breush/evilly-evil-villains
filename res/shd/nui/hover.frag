uniform sampler2D texture;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    vec4 original = texture2D(texture, texCoord);
    vec4 pixel = vec4(0.f);

    // Having range bigger to get a better effect
    // can slow the computer down
    const float step = 0.005;
    const int range = 3;

    // Blur
    for (int x = -range; x <= range; ++x)
    for (int y = -range; y <= range; ++y)
        pixel += texture2D(texture, texCoord + vec2(x, y) * step);

    // Adding original gives a bloom effect
    // by reinforcing the original colors
    gl_FragColor = original + (pixel / (4 * range * range));
}
