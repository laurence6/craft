#version 420 core

layout(location = 0) in vec3 vertex_p;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(vertex_p, 1.0);
}
