#version 420 core

layout(location = 0) in vec3 uv;
layout(location = 1) in float light;
layout(location = 2) flat in uint opaque;

layout(location = 0) out vec4 color;

uniform sampler2DArray sampler;

float dither2x2(vec2 position, vec3 color) {
    float brightness = dot(color, vec3(0.299, 0.587, 0.114));
    int x = int(mod(position.x, 2.0));
    int y = int(mod(position.y, 2.0));
    int index = x * 2 + y;
    float limit = 0.0;
    if (index == 0) limit = 0.25;
    if (index == 1) limit = 0.75;
    if (index == 2) limit = 1.00;
    if (index == 3) limit = 0.50;
    return brightness < limit ? 0.0 : 1.0;
}

void main() {
    vec4 tex_color = texture(sampler, uv);
    if (opaque == 0 || gl_FragCoord.w / gl_FragCoord.z < 0.01)
        tex_color *= dither2x2(gl_FragCoord.xy, tex_color.rgb);
    if (tex_color.a < 0.5)
        discard;

    color = vec4(tex_color.rgb * light, tex_color.a);
}
