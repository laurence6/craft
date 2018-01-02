#version 420 core

layout(location = 0) in vec2 vertex_p;

void main() {
    gl_Position = vec4(vertex_p, 0.0, 1.0);
}
