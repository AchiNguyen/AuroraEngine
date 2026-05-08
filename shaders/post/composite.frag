#version 460 core

in vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_hdr_color;
uniform sampler2D u_bloom_color;
uniform float     u_exposure;
uniform float     u_bloom_intensity;
uniform float     u_gamma;

// ACES filmic tonemapping (Narkowicz 2015 fit of the ACES curve).
vec3 aces_tonemap(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec3 hdr   = texture(u_hdr_color,   v_uv).rgb;
    vec3 bloom = texture(u_bloom_color, v_uv).rgb;
    vec3 mixed = hdr + bloom * u_bloom_intensity;
    vec3 mapped = aces_tonemap(mixed * u_exposure);
    vec3 gamma_corrected = pow(mapped, vec3(1.0 / u_gamma));
    frag_color = vec4(gamma_corrected, 1.0);
}
