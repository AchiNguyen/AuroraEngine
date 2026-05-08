#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;   // unused but kept for layout consistency
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec2 a_uv;

out vec3 v_world_position;
out vec3 v_world_normal;
out vec3 v_world_tangent;
out vec3 v_world_bitangent;
out vec2 v_uv;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    vec4 world_pos     = u_model * vec4(a_position, 1.0);
    v_world_position   = world_pos.xyz;

    mat3 normal_matrix = mat3(transpose(inverse(u_model)));
    v_world_normal     = normalize(normal_matrix * a_normal);
    v_world_tangent    = normalize(normal_matrix * a_tangent);
    // Gram-Schmidt: re-orthogonalize tangent against the (re-normalized) normal,
    // then derive the bitangent as their cross product.
    v_world_tangent    = normalize(v_world_tangent
                                   - dot(v_world_tangent, v_world_normal) * v_world_normal);
    v_world_bitangent  = cross(v_world_normal, v_world_tangent);

    v_uv        = a_uv;
    gl_Position = u_projection * u_view * world_pos;
}
