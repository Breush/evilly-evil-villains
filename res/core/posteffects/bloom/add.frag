uniform sampler2D source1;
uniform sampler2D source2;

void main()
{
    vec4 source1Fragment = texture2D(source1, gl_TexCoord[0].xy);
    vec4 source2Fragment = texture2D(source2, gl_TexCoord[0].xy);
    gl_FragColor = source1Fragment + source2Fragment;
}
