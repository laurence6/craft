#version 420 core

layout(location = 0) in vec3 vertex_p;
layout(location = 1) in vec3 vertex_uv;

layout(location = 0) out vec3 uv;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(vertex_p, 1.0);
    uv = vertex_uv;
}
