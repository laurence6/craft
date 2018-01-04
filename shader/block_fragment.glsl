#version 420 core

layout(location = 0) in vec3 uv;
layout(location = 1) in float light;

layout(location = 0) out vec4 color;

uniform sampler2DArray sampler;

void main() {
    vec4 tex_color = texture(sampler, uv);
    if (tex_color.a < 0.1) {
        discard;
    }
    color = vec4(tex_color.rgb * light, tex_color.a);
}
