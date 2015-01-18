uniform sampler2D texture;
uniform vec2 textureSize;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    vec2 step = vec2(1) / textureSize;
    vec4 pixel = vec4(0.f);

    // The original one
    pixel += texture2D(texture, texCoord);

    // Add power given nearby pixels
    pixel += texture2D(texture, texCoord + vec2(-1.f,  0.f) * step) * 0.7;
    pixel += texture2D(texture, texCoord + vec2( 1.f,  0.f) * step) * 0.7;
    pixel += texture2D(texture, texCoord + vec2( 0.f, -1.f) * step) * 0.7;
    pixel += texture2D(texture, texCoord + vec2( 0.f,  1.f) * step) * 0.7;

    pixel += texture2D(texture, texCoord + vec2(-1.f, -1.f) * step) * 0.4;
    pixel += texture2D(texture, texCoord + vec2( 1.f, -1.f) * step) * 0.4;
    pixel += texture2D(texture, texCoord + vec2(-1.f,  1.f) * step) * 0.4;
    pixel += texture2D(texture, texCoord + vec2( 1.f,  1.f) * step) * 0.4;

    // Clamping
    if (pixel.r > 1.f) pixel.r = 1.f;
    if (pixel.g > 1.f) pixel.g = 1.f;
    if (pixel.b > 1.f) pixel.b = 1.f;
    if (pixel.a > 1.f) pixel.a = 1.f;

    gl_FragColor = gl_Color * pixel;
}

