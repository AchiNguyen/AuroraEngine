#version 460 core

in vec3 v_color;
in vec3 v_normal;

out vec4 frag_color;

void main() {
    vec3 light_dir = normalize(vec3(0.5, 1.0, 0.3));
    float ndotl = max(dot(normalize(v_normal), light_dir), 0.0);
    float ambient = 0.25;
    frag_color = vec4(v_color * (ambient + ndotl * 0.75), 1.0);
}
