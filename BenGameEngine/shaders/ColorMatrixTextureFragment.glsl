#extension GL_EXT_shader_framebuffer_fetch : require

varying lowp vec2 TexCoordOut;
uniform sampler2D Texture;
uniform lowp mat4 ColorMatrix;
uniform lowp vec4 ColorMatOffset;

void main()
{
    lowp vec4 textureColor = texture2D(Texture, TexCoordOut);
    lowp vec4 frag = (textureColor * ColorMatrix);
    lowp vec4 destColor = gl_LastFragData[0];

    if (textureColor.a > 0.0) {
        frag = frag + ColorMatOffset;
    }

    gl_FragColor =  frag * frag.a + destColor * ( 1.0 - frag.a);
}
