#version 420 core

layout(location = 0) in vec3 vertex_p;
layout(location = 1) in vec3 vertex_uv;
layout(location = 2) in vec3 vertex_n;

uniform mat4 MVP;
uniform vec3 sun_dir;

layout(location = 0) out vec3 uv;
layout(location = 1) out float light;

void main() {
    gl_Position = MVP * vec4(vertex_p, 1.0);
    uv = vertex_uv;
    light = clamp(dot(sun_dir, vertex_n), 0.6, 1.0);
}
