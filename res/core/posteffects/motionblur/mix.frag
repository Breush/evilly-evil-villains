uniform sampler2D source1;
uniform sampler2D source2;

const float factor = 0.25f;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    vec4 color1 = texture2D(source1, coord);
    vec4 color2 = texture2D(source2, coord);
    gl_FragColor = factor * color1 + (1.f - factor) * color2;
}
