uniform float time;
uniform vec2 screenSize;
uniform float textureLength;

void main()
{
    vec4 vertex = gl_Vertex;
    mat2 rot = mat2(cos(time), sin(time), -sin(time), cos(time));

    // Rotation from center
    vertex.xy -= screenSize / 2.0;
    vertex.xy  = rot * vertex.xy;
    vertex.xy += screenSize / 2.0;

	gl_Position = gl_ModelViewProjectionMatrix * vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
}
