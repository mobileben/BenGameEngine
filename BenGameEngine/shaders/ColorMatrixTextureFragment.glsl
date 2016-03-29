#extension GL_EXT_shader_framebuffer_fetch : require

varying lowp vec2 TexCoordOut;
uniform sampler2D Texture;
uniform lowp mat4 colorMatrix;
uniform lowp vec4 colorOffset;

void main()
{
    lowp vec4 textureColor = texture2D(Texture, TexCoordOut);
    lowp vec4 frag = (textureColor * colorMatrix) + colorOffset;
    lowp vec4 destColor = gl_LastFragData[0];
    
    gl_FragColor =  frag * frag.a + destColor * ( 1.0 - frag.a);
}