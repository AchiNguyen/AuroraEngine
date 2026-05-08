#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>

namespace aurora {

class Framebuffer {
public:
    static Framebuffer depth_only(int width, int height);
    static Framebuffer hdr_color(int width, int height, bool with_depth);

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) noexcept;
    Framebuffer& operator=(Framebuffer&&) noexcept;
    ~Framebuffer();

    void bind()   const;
    void unbind() const;

    void resize(int width, int height);

    GLuint     fbo()              const { return fbo_; }
    GLuint     color_texture_id() const { return color_tex_; }
    GLuint     depth_texture_id() const { return depth_tex_; }
    glm::ivec2 size()             const { return size_; }

private:
    enum class Kind { DepthOnly, HdrColor };

    Framebuffer() = default;
    void release() noexcept;
    void release_attachments() noexcept;
    void allocate_attachments(int width, int height);

    Kind       kind_      = Kind::DepthOnly;
    bool       has_depth_ = false;
    GLuint     fbo_       = 0;
    GLuint     color_tex_ = 0;
    GLuint     depth_tex_ = 0;
    glm::ivec2 size_{0, 0};
};

}
