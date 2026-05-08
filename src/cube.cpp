#include "aurora/cube.hpp"

#include <array>
#include <cstdint>

namespace aurora {

namespace {

// Per-face primary colors (kept for layout/attribute consistency, ignored by the
// textured shader; useful for non-textured debug shaders).
constexpr glm::vec3 kColorPosX{1.0f, 0.0f, 0.0f};
constexpr glm::vec3 kColorNegX{0.0f, 1.0f, 1.0f};
constexpr glm::vec3 kColorPosY{0.0f, 1.0f, 0.0f};
constexpr glm::vec3 kColorNegY{1.0f, 0.0f, 1.0f};
constexpr glm::vec3 kColorPosZ{0.0f, 0.0f, 1.0f};
constexpr glm::vec3 kColorNegZ{1.0f, 1.0f, 0.0f};

// Each face's 4 corners get the unit square (0,0)(1,0)(1,1)(0,1) so the full
// image is mapped per face.
constexpr glm::vec2 kUv00{0.0f, 0.0f};
constexpr glm::vec2 kUv10{1.0f, 0.0f};
constexpr glm::vec2 kUv11{1.0f, 1.0f};
constexpr glm::vec2 kUv01{0.0f, 1.0f};

constexpr std::array<Vertex, 24> kVertices{{
    // +Z front face (normal +Z)
    {{-0.5f, -0.5f,  0.5f}, kColorPosZ, { 0.0f,  0.0f,  1.0f}, kUv00},
    {{ 0.5f, -0.5f,  0.5f}, kColorPosZ, { 0.0f,  0.0f,  1.0f}, kUv10},
    {{ 0.5f,  0.5f,  0.5f}, kColorPosZ, { 0.0f,  0.0f,  1.0f}, kUv11},
    {{-0.5f,  0.5f,  0.5f}, kColorPosZ, { 0.0f,  0.0f,  1.0f}, kUv01},

    // -Z back face (normal -Z)
    {{ 0.5f, -0.5f, -0.5f}, kColorNegZ, { 0.0f,  0.0f, -1.0f}, kUv00},
    {{-0.5f, -0.5f, -0.5f}, kColorNegZ, { 0.0f,  0.0f, -1.0f}, kUv10},
    {{-0.5f,  0.5f, -0.5f}, kColorNegZ, { 0.0f,  0.0f, -1.0f}, kUv11},
    {{ 0.5f,  0.5f, -0.5f}, kColorNegZ, { 0.0f,  0.0f, -1.0f}, kUv01},

    // +X right face (normal +X)
    {{ 0.5f, -0.5f,  0.5f}, kColorPosX, { 1.0f,  0.0f,  0.0f}, kUv00},
    {{ 0.5f, -0.5f, -0.5f}, kColorPosX, { 1.0f,  0.0f,  0.0f}, kUv10},
    {{ 0.5f,  0.5f, -0.5f}, kColorPosX, { 1.0f,  0.0f,  0.0f}, kUv11},
    {{ 0.5f,  0.5f,  0.5f}, kColorPosX, { 1.0f,  0.0f,  0.0f}, kUv01},

    // -X left face (normal -X)
    {{-0.5f, -0.5f, -0.5f}, kColorNegX, {-1.0f,  0.0f,  0.0f}, kUv00},
    {{-0.5f, -0.5f,  0.5f}, kColorNegX, {-1.0f,  0.0f,  0.0f}, kUv10},
    {{-0.5f,  0.5f,  0.5f}, kColorNegX, {-1.0f,  0.0f,  0.0f}, kUv11},
    {{-0.5f,  0.5f, -0.5f}, kColorNegX, {-1.0f,  0.0f,  0.0f}, kUv01},

    // +Y top face (normal +Y)
    {{-0.5f,  0.5f,  0.5f}, kColorPosY, { 0.0f,  1.0f,  0.0f}, kUv00},
    {{ 0.5f,  0.5f,  0.5f}, kColorPosY, { 0.0f,  1.0f,  0.0f}, kUv10},
    {{ 0.5f,  0.5f, -0.5f}, kColorPosY, { 0.0f,  1.0f,  0.0f}, kUv11},
    {{-0.5f,  0.5f, -0.5f}, kColorPosY, { 0.0f,  1.0f,  0.0f}, kUv01},

    // -Y bottom face (normal -Y)
    {{-0.5f, -0.5f, -0.5f}, kColorNegY, { 0.0f, -1.0f,  0.0f}, kUv00},
    {{ 0.5f, -0.5f, -0.5f}, kColorNegY, { 0.0f, -1.0f,  0.0f}, kUv10},
    {{ 0.5f, -0.5f,  0.5f}, kColorNegY, { 0.0f, -1.0f,  0.0f}, kUv11},
    {{-0.5f, -0.5f,  0.5f}, kColorNegY, { 0.0f, -1.0f,  0.0f}, kUv01},
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
