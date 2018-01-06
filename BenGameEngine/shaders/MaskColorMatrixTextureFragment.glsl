#extension GL_EXT_shader_framebuffer_fetch : require

varying lowp vec2 TexCoordOut;
varying lowp vec2 MaskTexCoordOut;
uniform sampler2D Texture;
uniform sampler2D MaskTexture;
uniform lowp mat4 ColorMatrix;
uniform lowp vec4 ColorMatOffset;

void main()
{
    lowp vec4 textureColor = texture2D(Texture, TexCoordOut);
    lowp float maskAlpha = texture2D(MaskTexture, MaskTexCoordOut).a;
    lowp vec4 frag = (textureColor * ColorMatrix) + ColorMatOffset;
    lowp vec4 destColor = gl_LastFragData[0];

    gl_FragColor = vec4(frag.rgb * maskAlpha + destColor.rgb * (1.0 - maskAlpha), maskAlpha);
    //gl_FragColor =  frag * frag.a + destColor * ( 1.0 - frag.a);
}
