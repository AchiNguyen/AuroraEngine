#pragma once

#include "aurora/shader.hpp"

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include <memory>

namespace aurora {

class Cubemap;

class Skybox {
public:
    explicit Skybox(std::shared_ptr<Cubemap> cubemap);
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;
    Skybox(Skybox&&) noexcept;
    Skybox& operator=(Skybox&&) noexcept;

    void draw(const glm::mat4& view, const glm::mat4& projection, float brightness);

    Cubemap*       cubemap()       { return cubemap_.get(); }
    const Cubemap* cubemap() const { return cubemap_.get(); }

private:
    void release_buffers() noexcept;

    std::shared_ptr<Cubemap> cubemap_;
    Shader                   shader_;
    GLuint                   vao_ = 0;
    GLuint                   vbo_ = 0;
};

}
