#include "aurora/plane.hpp"

#include <array>
#include <cstdint>

namespace aurora {

Mesh make_plane(float size_x, float size_z) {
    const float hx = size_x * 0.5f;
    const float hz = size_z * 0.5f;
    const glm::vec3 normal {0.0f, 1.0f, 0.0f};
    const glm::vec3 tangent{1.0f, 0.0f, 0.0f};
    const glm::vec3 white  {1.0f, 1.0f, 1.0f};

    const std::array<Vertex, 4> vertices{
        Vertex{{-hx, 0.0f, -hz}, white, normal, tangent, {0.0f, 0.0f}},
        Vertex{{ hx, 0.0f, -hz}, white, normal, tangent, {1.0f, 0.0f}},
        Vertex{{ hx, 0.0f,  hz}, white, normal, tangent, {1.0f, 1.0f}},
        Vertex{{-hx, 0.0f,  hz}, white, normal, tangent, {0.0f, 1.0f}},
    };
    const std::array<std::uint32_t, 6> indices{0, 1, 2, 0, 2, 3};

    return Mesh(vertices, indices);
}

}
