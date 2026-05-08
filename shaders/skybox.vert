#version 460 core

layout(location = 0) in vec3 a_position;

out vec3 v_local_dir;

uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    v_local_dir = a_position;
    vec4 pos    = u_projection * u_view * vec4(a_position, 1.0);
    gl_Position = pos.xyww;
}
