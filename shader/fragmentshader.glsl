#version 420 core

in vec3 uv;

out vec3 color;

uniform sampler2DArray sampler;

void main() {
    color = texture(sampler, uv).rgb;
}
