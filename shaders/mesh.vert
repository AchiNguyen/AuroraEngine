#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec3 a_normal;

out vec3 v_world_position;
out vec3 v_world_normal;
out vec3 v_color;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    vec4 world_pos = u_model * vec4(a_position, 1.0);
    v_world_position = world_pos.xyz;
    v_world_normal = mat3(transpose(inverse(u_model))) * a_normal;
    v_color = a_color;
    gl_Position = u_projection * u_view * world_pos;
}
