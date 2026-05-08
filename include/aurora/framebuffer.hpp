#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>

namespace aurora {

class Framebuffer {
public:
    static Framebuffer depth_only(int width, int height);

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) noexcept;
    Framebuffer& operator=(Framebuffer&&) noexcept;
    ~Framebuffer();

    void bind()   const;
    void unbind() const;

    GLuint     fbo()              const { return fbo_; }
    GLuint     depth_texture_id() const { return depth_tex_; }
    glm::ivec2 size()             const { return size_; }

private:
    Framebuffer() = default;
    void release() noexcept;

    GLuint     fbo_       = 0;
    GLuint     depth_tex_ = 0;
    glm::ivec2 size_{0, 0};
};

}
