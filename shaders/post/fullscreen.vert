#version 460 core

out vec2 v_uv;

void main() {
    // Single full-screen triangle covering NDC.
    // Vertex 0: (-1,-1), 1: (3,-1), 2: (-1,3) — clipped to the [-1,1] viewport.
    vec2 positions[3] = vec2[](vec2(-1.0, -1.0), vec2(3.0, -1.0), vec2(-1.0, 3.0));
    vec2 pos = positions[gl_VertexID];
    v_uv = pos * 0.5 + 0.5;
    gl_Position = vec4(pos, 0.0, 1.0);
}
