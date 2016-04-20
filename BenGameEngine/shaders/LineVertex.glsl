attribute vec4 Position;

uniform mat4 Projection;
uniform mat4 ModelView;

void main(void) {
    gl_Position = Projection * Position;
}