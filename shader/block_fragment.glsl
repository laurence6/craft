#version 420 core

layout(location = 0) in vec3 uv;
layout(location = 1) in float light;

layout(location = 0) out vec4 color;

uniform sampler2DArray sampler;

const vec4 fog_color = vec4(0.5, 0.6, 0.7, 1.0);

void main() {
    vec4 tex_color = texture(sampler, uv);
    if (tex_color.a < 0.1) {
        discard;
    }

    float fog = clamp(gl_FragCoord.w / gl_FragCoord.z + 20., 0.0, 1.0);
    fog = pow(fog, 8);

    color = vec4(tex_color.rgb * light, tex_color.a);
    color *= fog;
    color += fog_color * (1.0 - fog);
}
