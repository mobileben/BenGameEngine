uniform lowp vec4 SourceColor;
varying lowp vec2 TexCoordOut;
uniform sampler2D Texture;

void main(void) {
    gl_FragColor = texture2D(Texture, TexCoordOut).aaaa * SourceColor;
    //gl_FragColor = frag;
//    if (gl_FragColor.a < 0.9 && gl_FragColor.a != 0.0) {
//        gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
//    }
}

