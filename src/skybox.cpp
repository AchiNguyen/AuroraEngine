#include "aurora/skybox.hpp"

#include "aurora/cubemap.hpp"

#include <glm/mat3x3.hpp>

#include <spdlog/spdlog.h>

#include <utility>

namespace aurora {

namespace {

constexpr float kSkyboxVertices[] = {
    // +X
     1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
    // -X
    -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
    // +Y
    -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,
    // -Y
    -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f,
    // +Z
    -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
    // -Z
    -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,
};

}

Skybox::Skybox(std::shared_ptr<Cubemap> cubemap)
    : cubemap_(std::move(cubemap)),
      shader_("shaders/skybox.vert", "shaders/skybox.frag") {
    glGenVertexArrays(1, &vao_);
    glGenBuffers     (1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(kSkyboxVertices),
                 kSkyboxVertices,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    spdlog::info("Skybox initialized.");
}

Skybox::~Skybox() {
    release_buffers();
}

Skybox::Skybox(Skybox&& other) noexcept
    : cubemap_(std::move(other.cubemap_)),
      shader_ (std::move(other.shader_)),
      vao_    (std::exchange(other.vao_, 0)),
      vbo_    (std::exchange(other.vbo_, 0)) {}

Skybox& Skybox::operator=(Skybox&& other) noexcept {
    if (this != &other) {
        release_buffers();
        cubemap_ = std::move(other.cubemap_);
        shader_  = std::move(other.shader_);
        vao_     = std::exchange(other.vao_, 0);
        vbo_     = std::exchange(other.vbo_, 0);
    }
    return *this;
}

void Skybox::release_buffers() noexcept {
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
}

void Skybox::draw(const glm::mat4& view, const glm::mat4& projection, float brightness) {
    glDepthFunc(GL_LEQUAL);

    shader_.bind();
    const glm::mat4 view_no_translate = glm::mat4(glm::mat3(view));
    shader_.set_uniform("u_view",       view_no_translate);
    shader_.set_uniform("u_projection", projection);
    shader_.set_uniform("u_brightness", brightness);

    if (cubemap_) {
        cubemap_->bind(0);
        shader_.set_uniform("u_skybox", 0);
    }

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}

}
