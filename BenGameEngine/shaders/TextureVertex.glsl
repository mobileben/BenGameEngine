attribute vec4 Position;

uniform mat4 Projection;
uniform mat4 ModelView;

attribute vec2 TexCoordIn;
varying vec2 TexCoordOut;

void main(void) {
    gl_Position = Projection * ModelView * Position;
    TexCoordOut = TexCoordIn;
}