#include "aurora/framebuffer.hpp"

#include <spdlog/spdlog.h>

#include <stdexcept>
#include <utility>

namespace aurora {

Framebuffer Framebuffer::depth_only(int width, int height) {
    Framebuffer f;
    f.size_ = {width, height};

    glGenTextures(1, &f.depth_tex_);
    glBindTexture(GL_TEXTURE_2D, f.depth_tex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
                 width, height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_BORDER);
    constexpr float kBorder[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, kBorder);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &f.fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, f.fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, f.depth_tex_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        f.release();
        spdlog::error("Framebuffer (depth-only) incomplete: 0x{:X}", status);
        throw std::runtime_error("Framebuffer (depth-only) is not complete");
    }

    spdlog::info("Framebuffer (depth-only) {}x{} fbo={} depth_tex={}",
                 width, height, f.fbo_, f.depth_tex_);
    return f;
}

Framebuffer::~Framebuffer() {
    release();
}

Framebuffer::Framebuffer(Framebuffer&& o) noexcept
    : fbo_      (std::exchange(o.fbo_,       0)),
      depth_tex_(std::exchange(o.depth_tex_, 0)),
      size_     (o.size_) {}

Framebuffer& Framebuffer::operator=(Framebuffer&& o) noexcept {
    if (this != &o) {
        release();
        fbo_       = std::exchange(o.fbo_,       0);
        depth_tex_ = std::exchange(o.depth_tex_, 0);
        size_      = o.size_;
    }
    return *this;
}

void Framebuffer::release() noexcept {
    if (fbo_       != 0) { glDeleteFramebuffers(1, &fbo_);     fbo_       = 0; }
    if (depth_tex_ != 0) { glDeleteTextures   (1, &depth_tex_); depth_tex_ = 0; }
    size_ = {0, 0};
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
