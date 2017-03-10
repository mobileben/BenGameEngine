#extension GL_EXT_shader_framebuffer_fetch : require

uniform lowp vec4 SourceColor;
varying lowp vec2 TexCoordOut;
uniform sampler2D Texture;
uniform lowp mat4 ColorMatrix;
uniform lowp vec4 ColorMatOffset;
uniform lowp vec4 ColorMultiplier;
uniform lowp vec4 ColorOffset;

void main(void) {
    lowp vec4 destColor = gl_LastFragData[0];
    lowp vec4 frag = texture2D(Texture, TexCoordOut).aaaa * SourceColor;
   
    frag = (frag * ColorMatrix) + ColorMatOffset;
    frag = (frag * ColorMultiplier) + ColorOffset;
    
    frag = clamp(frag, vec4(0.0, 0.0, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0));
    
    gl_FragColor =  frag * frag.a + destColor * ( 1.0 - frag.a);
}

