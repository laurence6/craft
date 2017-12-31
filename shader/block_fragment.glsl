#version 420 core

in vec3 uv;

out vec4 color;

uniform sampler2DArray sampler;

void main() {
    color = texture(sampler, uv);
    if (color.a < 0.1) {
        discard;
    }
}
