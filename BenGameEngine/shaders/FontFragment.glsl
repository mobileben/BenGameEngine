uniform lowp vec4 SourceColor;
varying lowp vec2 TexCoordOut;
uniform sampler2D Texture;

void main(void) {
//    gl_FragColor = texture2D(Texture, TexCoordOut).aaaa ;
    gl_FragColor = texture2D(Texture, TexCoordOut).aaaa * SourceColor;
}

