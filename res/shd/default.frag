uniform sampler2D texture;

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    gl_FragColor = gl_Color * vec4(1.f, 0.5f, 0.5f, 1.f) * pixel;
}

