attribute vec4 Position;
attribute vec4 SourceColor;

uniform mat4 Projection;
uniform mat4 ModelView;

varying vec4 Color;

void main(void) {
    gl_Position = Projection * ModelView * Position;
    Color = SourceColor;
}
