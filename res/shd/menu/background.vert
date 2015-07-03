uniform float time;
uniform vec2 textureSize;
uniform vec2 resolution;

void main()
{
    vec4 vertex = gl_Vertex;

    // Rotation from center
    vertex.y -= (textureSize.y - resolution.y) * cos(time) * cos(time);

    gl_Position = gl_ModelViewProjectionMatrix * vertex;
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}
