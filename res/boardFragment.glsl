#version 300 es

out lowp vec4 color;

in lowp vec2 UV;

void main() {
    int board = int(UV.x * 8.f) + int(UV.y * 8.f);
    lowp float c = float(board & 1)*0.6+0.3;
    color = vec4(c, c, c, 1);
}