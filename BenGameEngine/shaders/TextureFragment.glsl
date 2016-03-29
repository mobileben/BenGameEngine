//#extension GL_EXT_shader_framebuffer_fetch : require

varying lowp vec2 TexCoordOut;
uniform sampler2D Texture;

void main(void) {
//    lowp vec4 textureColor = texture2D(Texture, TexCoordOut);
    gl_FragColor = texture2D(Texture, TexCoordOut);
    
    if (gl_FragColor.a == 0.0) {
        discard;
    }
}