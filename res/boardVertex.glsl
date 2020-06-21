#version 300 es

in vec2 vertexPosition;

out vec2 UV;

void main() {
    UV = (vertexPosition + vec2(1)) * 0.5f;
    gl_Position.xy = vertexPosition;
    gl_Position.zw = vec2(1.f, 1.f);
}