attribute vec4 Position;

uniform mat4 Projection;
uniform mat4 ModelView;

attribute vec2 TexCoordIn;
varying vec2 TexCoordOut;
attribute vec2 MaskTexCoordIn;
varying vec2 MaskTexCoordOut;

void main(void) {
    gl_Position = Projection * Position;
    TexCoordOut = TexCoordIn;
    MaskTexCoordOut = MaskTexCoordIn;
}