uniform sampler2D source;

const float threshold = 0.7;
const float factor   = 4.0;

void main()
{
    vec4 sourceFragment = texture2D(source, gl_TexCoord[0].xy);
    float luminance = sourceFragment.r * 0.2126 + sourceFragment.g * 0.7152 + sourceFragment.b * 0.0722;
    sourceFragment *= clamp(luminance - threshold, 0.0, 1.0) * factor;
    gl_FragColor = sourceFragment;
}

