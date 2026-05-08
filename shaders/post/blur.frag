#version 460 core

in vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_input;
uniform vec2      u_direction;
uniform float     u_radius;

void main() {
    vec2  texel  = 1.0 / vec2(textureSize(u_input, 0));
    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    vec3 result = texture(u_input, v_uv).rgb * weights[0];
    for (int i = 1; i < 5; ++i) {
        vec2 offset = u_direction * texel * float(i) * u_radius;
        result += texture(u_input, v_uv + offset).rgb * weights[i];
        result += texture(u_input, v_uv - offset).rgb * weights[i];
    }
    frag_color = vec4(result, 1.0);
}
