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
    lowp vec4 texel = texture2D(Texture, TexCoordOut);
    lowp vec4 frag = vec4(SourceColor.rgb, texel.a * SourceColor.a);

    frag = (frag * ColorMatrix) + ColorMatOffset;
    frag = (frag * ColorMultiplier) + ColorOffset;
    
    frag = clamp(frag, vec4(0.0, 0.0, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0));

    //gl_FragColor = mix(destColor, frag, frag.a);
    gl_FragColor = vec4(frag.rgb * frag.a + destColor.rgb * (1.0 - frag.a), frag.a);
}

