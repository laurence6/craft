#version 420 core

layout(location = 0) in vec3 vertex_p;

void main() {
    gl_Position = vec4(vertex_p, 1.0);
}
