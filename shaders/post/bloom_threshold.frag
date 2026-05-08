#version 460 core

in vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_hdr_color;
uniform float     u_threshold;
uniform float     u_soft_knee;

void main() {
    vec3  c          = texture(u_hdr_color, v_uv).rgb;
    float brightness = max(c.r, max(c.g, c.b));

    float knee = u_threshold * u_soft_knee + 1e-5;
    float soft = clamp(brightness - u_threshold + knee, 0.0, 2.0 * knee);
    soft       = soft * soft / (4.0 * knee);

    float contribution = max(soft, brightness - u_threshold);
    contribution      /= max(brightness, 1e-5);

    frag_color = vec4(c * contribution, 1.0);
}
