#version 420 core

layout(location = 0) in vec3 vertex_p;
layout(location = 1) in uint vertex_param;

uniform mat4 MVP;
uniform vec3 sun_dir;
uniform vec3 normals[6];

layout(location = 0) out vec3 uv;
layout(location = 1) out float light;

void main() {
    gl_Position = MVP * vec4(vertex_p, 1.0);
    uv = vec3(
        float((vertex_param & (1u << 28)) >> 28),
        float((vertex_param & (1u << 27)) >> 27),
        float(vertex_param & ((1u << 27) - 1))
    );
    vec3 vertex_n = normals[(vertex_param & 0xe0000000u) >> 29];
    light = clamp(dot(sun_dir, vertex_n), 0.6, 1.0);
}
