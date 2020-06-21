#version 300 es

in lowp vec2 UV;

out lowp vec4 color;

uniform sampler2D tex;

void main() {
    color = texture(tex, UV);
}

