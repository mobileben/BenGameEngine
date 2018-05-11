#extension GL_EXT_shader_framebuffer_fetch : require

varying lowp vec2 TexCoordOut;
uniform sampler2D Texture;
uniform lowp mat4 ColorMatrix;
uniform lowp vec4 ColorMatOffset;
uniform lowp vec4 ColorMultiplier;
uniform lowp vec4 ColorOffset;

void main()
{
    lowp vec4 textureColor = texture2D(Texture, TexCoordOut);
    lowp vec4 frag = (textureColor * ColorMatrix);
    lowp vec4 destColor = gl_LastFragData[0];

    if (textureColor.a > 0.0) {
        frag = frag + ColorMatOffset;
    }
    
    frag = (frag * ColorMultiplier);
    if (frag.a > 0.0) {
        frag = frag + ColorOffset;
    }
    // Look at https://stackoverflow.com/questions/5713830/colortransform-equivalent-color-matrix
    frag = clamp(frag, vec4(0.0, 0.0, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0));
    
    gl_FragColor =  frag * frag.a + destColor * ( 1.0 - frag.a);
}
