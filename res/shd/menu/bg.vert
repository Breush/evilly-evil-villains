uniform float time;
uniform vec2 resolution;

void main()
{
    vec4 vertex = gl_Vertex;
    mat2 rot = mat2(cos(time), sin(time), -sin(time), cos(time));

	vec2 halfResolution = resolution / 2.f;

    // Rotation from center
    vertex.xy -= halfResolution;
    vertex.xy  = rot * vertex.xy;
    vertex.xy += halfResolution;

	gl_Position = gl_ModelViewProjectionMatrix * vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
}
