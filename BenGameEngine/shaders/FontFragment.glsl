#extension GL_EXT_shader_framebuffer_fetch : require

uniform lowp vec4 SourceColor;
varying lowp vec2 TexCoordOut;
uniform sampler2D Texture;

void main(void) {
    lowp vec4 destColor = gl_LastFragData[0];
    lowp vec4 frag = texture2D(Texture, TexCoordOut).aaaa * SourceColor;
    
    gl_FragColor = frag + destColor * ( 1.0 - frag.a);
}

