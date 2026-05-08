#version 460 core

in vec3 v_local_dir;

out vec4 frag_color;

uniform samplerCube u_skybox;
uniform float       u_brightness;

void main() {
    frag_color = texture(u_skybox, v_local_dir) * u_brightness;
}
