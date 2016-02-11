uniform sampler2D source1;
uniform sampler2D source2;

const float factor = 0.3f;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    vec4 source1Fragment = texture2D(source1, coord);
    float cliffValue = 1.f - factor * texture2D(source2, coord).r;

    // Mixing the source2 (sobel filter) as dark border
    gl_FragColor = vec4(cliffValue * source1Fragment.rgb, source1Fragment.a);
}
