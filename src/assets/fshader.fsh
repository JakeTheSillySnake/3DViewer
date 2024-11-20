uniform sampler2D qt_Texture0;
varying highp vec2 qt_TexCoord0;

in vec4 vertexColor;

void main(void)
{
    gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0);
}
