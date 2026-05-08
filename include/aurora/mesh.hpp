#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstdint>
#include <span>

namespace aurora {

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 uv;
};

class Mesh {
public:
    Mesh(std::span<const Vertex> vertices, std::span<const std::uint32_t> indices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;

    void draw() const;

    GLsizei index_count() const { return index_count_; }

private:
    void release() noexcept;

    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;
    GLsizei index_count_ = 0;
};

}
