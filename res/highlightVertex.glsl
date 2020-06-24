#version 300 es

in vec2 vertexPosition;

void main() {
    vec2 pos = vertexPosition * 2.f - vec2(1);
    gl_Position = vec4(pos, 0, 1);
}
