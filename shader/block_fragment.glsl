#version 420 core

layout(location = 0) in vec3 uv;

layout(location = 0) out vec4 color;

uniform sampler2DArray sampler;

void main() {
    color = texture(sampler, uv);
    if (color.a < 0.1) {
        discard;
    }
}
