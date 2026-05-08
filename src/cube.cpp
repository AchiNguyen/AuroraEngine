#include "aurora/cube.hpp"

#include <array>
#include <cstdint>

namespace aurora {

namespace {

// Per-face primary colors so each face is visually distinct.
constexpr glm::vec3 kColorPosX{1.0f, 0.0f, 0.0f}; // right   = red
constexpr glm::vec3 kColorNegX{0.0f, 1.0f, 1.0f}; // left    = cyan
constexpr glm::vec3 kColorPosY{0.0f, 1.0f, 0.0f}; // top     = green
constexpr glm::vec3 kColorNegY{1.0f, 0.0f, 1.0f}; // bottom  = magenta
constexpr glm::vec3 kColorPosZ{0.0f, 0.0f, 1.0f}; // front   = blue
constexpr glm::vec3 kColorNegZ{1.0f, 1.0f, 0.0f}; // back    = yellow

constexpr std::array<Vertex, 24> kVertices{{
    // +Z front face (normal +Z, blue)
    {{-0.5f, -0.5f,  0.5f}, kColorPosZ, { 0.0f,  0.0f,  1.0f}},
    {{ 0.5f, -0.5f,  0.5f}, kColorPosZ, { 0.0f,  0.0f,  1.0f}},
    {{ 0.5f,  0.5f,  0.5f}, kColorPosZ, { 0.0f,  0.0f,  1.0f}},
    {{-0.5f,  0.5f,  0.5f}, kColorPosZ, { 0.0f,  0.0f,  1.0f}},

    // -Z back face (normal -Z, yellow)
    {{ 0.5f, -0.5f, -0.5f}, kColorNegZ, { 0.0f,  0.0f, -1.0f}},
    {{-0.5f, -0.5f, -0.5f}, kColorNegZ, { 0.0f,  0.0f, -1.0f}},
    {{-0.5f,  0.5f, -0.5f}, kColorNegZ, { 0.0f,  0.0f, -1.0f}},
    {{ 0.5f,  0.5f, -0.5f}, kColorNegZ, { 0.0f,  0.0f, -1.0f}},

    // +X right face (normal +X, red)
    {{ 0.5f, -0.5f,  0.5f}, kColorPosX, { 1.0f,  0.0f,  0.0f}},
    {{ 0.5f, -0.5f, -0.5f}, kColorPosX, { 1.0f,  0.0f,  0.0f}},
    {{ 0.5f,  0.5f, -0.5f}, kColorPosX, { 1.0f,  0.0f,  0.0f}},
    {{ 0.5f,  0.5f,  0.5f}, kColorPosX, { 1.0f,  0.0f,  0.0f}},

    // -X left face (normal -X, cyan)
    {{-0.5f, -0.5f, -0.5f}, kColorNegX, {-1.0f,  0.0f,  0.0f}},
    {{-0.5f, -0.5f,  0.5f}, kColorNegX, {-1.0f,  0.0f,  0.0f}},
    {{-0.5f,  0.5f,  0.5f}, kColorNegX, {-1.0f,  0.0f,  0.0f}},
    {{-0.5f,  0.5f, -0.5f}, kColorNegX, {-1.0f,  0.0f,  0.0f}},

    // +Y top face (normal +Y, green)
    {{-0.5f,  0.5f,  0.5f}, kColorPosY, { 0.0f,  1.0f,  0.0f}},
    {{ 0.5f,  0.5f,  0.5f}, kColorPosY, { 0.0f,  1.0f,  0.0f}},
    {{ 0.5f,  0.5f, -0.5f}, kColorPosY, { 0.0f,  1.0f,  0.0f}},
    {{-0.5f,  0.5f, -0.5f}, kColorPosY, { 0.0f,  1.0f,  0.0f}},

    // -Y bottom face (normal -Y, magenta)
    {{-0.5f, -0.5f, -0.5f}, kColorNegY, { 0.0f, -1.0f,  0.0f}},
    {{ 0.5f, -0.5f, -0.5f}, kColorNegY, { 0.0f, -1.0f,  0.0f}},
    {{ 0.5f, -0.5f,  0.5f}, kColorNegY, { 0.0f, -1.0f,  0.0f}},
    {{-0.5f, -0.5f,  0.5f}, kColorNegY, { 0.0f, -1.0f,  0.0f}},
}};

constexpr std::array<std::uint32_t, 36> kIndices{{
     0,  1,  2,    0,  2,  3,    // +Z
     4,  5,  6,    4,  6,  7,    // -Z
     8,  9, 10,    8, 10, 11,    // +X
    12, 13, 14,   12, 14, 15,    // -X
    16, 17, 18,   16, 18, 19,    // +Y
    20, 21, 22,   20, 22, 23,    // -Y
}};

}

Mesh make_cube() {
    return Mesh(std::span<const Vertex>(kVertices.data(),  kVertices.size()),
                std::span<const std::uint32_t>(kIndices.data(), kIndices.size()));
}

}
