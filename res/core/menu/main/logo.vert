uniform float time;

void main()
{
    vec4 vertex = gl_Vertex;
    vertex.x += cos(gl_Vertex.y * 0.02f + time * 3.8f) * 5.f
              + sin(gl_Vertex.y * 0.02f + time * 6.3f) * 5.f * 0.3f;
    vertex.y += sin(gl_Vertex.x * 0.02f + time * 2.4f) * 5.f
              + cos(gl_Vertex.x * 0.02f + time * 5.2f) * 5.f * 0.3f;

    gl_Position = gl_ModelViewProjectionMatrix * vertex;
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}
