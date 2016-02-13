/* Returns a grey-shaded sobel-filtered image. */

uniform sampler2D source;
uniform vec2 sourceSize;

uniform sampler2D floom;
uniform vec2 floomSize;
uniform vec2 floomOffset;

float luminosity(vec4 v)
{
    return (v.r + v.g + v.b) / 3.f;
}

float luminosity(vec2 coord, vec2 offset)
{
    return luminosity(texture2D(source, coord + offset));
}

void main(void)
{
    float x = 1.0 / sourceSize.x;
    float y = 1.0 / sourceSize.y;

    vec2 coord = gl_TexCoord[0].xy;
    vec4 color = texture2D(source, coord);
    vec2 floomCoord = (coord * sourceSize + floomOffset) / floomSize;
    float floomPower = texture2D(floom, floomCoord).x;

    float horizEdge = 0.0;
    horizEdge -= luminosity(coord, vec2(-x, -y)) * 1.0;
    horizEdge -= luminosity(coord, vec2(-x, 0.)) * 2.0;
    horizEdge -= luminosity(coord, vec2(-x,  y)) * 1.0;
    horizEdge += luminosity(coord, vec2( x, -y)) * 1.0;
    horizEdge += luminosity(coord, vec2( x, 0.)) * 2.0;
    horizEdge += luminosity(coord, vec2( x,  y)) * 1.0;

    float vertEdge = 0.0;
    vertEdge -= luminosity(coord, vec2(-x, -y)) * 1.0;
    vertEdge -= luminosity(coord, vec2(0., -y)) * 2.0;
    vertEdge -= luminosity(coord, vec2( x, -y)) * 1.0;
    vertEdge += luminosity(coord, vec2(-x,  y)) * 1.0;
    vertEdge += luminosity(coord, vec2(0.,  y)) * 2.0;
    vertEdge += luminosity(coord, vec2( x,  y)) * 1.0;

    float edge = sqrt((horizEdge * horizEdge) + (vertEdge * vertEdge));

    gl_FragColor = vec4(vec3(floomPower * edge), color.a);
}
