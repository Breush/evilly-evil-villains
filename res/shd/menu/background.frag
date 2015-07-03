uniform sampler2D texture;
uniform float time;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    vec4 original = texture2D(texture, texCoord);
    vec4 pixel = vec4(0.f);

    const float step = 0.001f;
    const int range = 3;

    for (int x = -range; x <= range; ++x)
    for (int y = -range; y <= range; ++y)
        pixel += texture2D(texture, texCoord + vec2(x, y) * step);

    // Compute the final value of blur
    const float additions = float(2 * range + 1) * float(2 * range + 1);
    vec4 finalColor = gl_Color * (original + pixel / additions);

    // TODO Add some fog...
    // This make colors variate in time (we use prime numbers to get no-sync effect)
    /*vec4 variationColor = vec4(texCoord.x, texCoord.x, texCoord.x, 0.f);
    variationColor *= time * vec4(5.f, 13.f, 23.f, 0.f);
    finalColor *= cos(variationColor) * cos(variationColor);*/

    gl_FragColor = finalColor;
}
