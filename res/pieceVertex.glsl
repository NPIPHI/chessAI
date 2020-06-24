#version 300 es

in vec2 vertexPosition;
in vec2 uv;

out vec2 UV;

void main() {
    UV = uv;
    vec2 pos = vertexPosition * 2.f - vec2(1);
    gl_Position = vec4(pos, 0.5, 1);
}
