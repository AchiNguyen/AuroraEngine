#include "aurora/framebuffer.hpp"

#include <spdlog/spdlog.h>

#include <stdexcept>
#include <utility>

namespace aurora {

namespace {

void check_complete(GLuint fbo, const char* tag) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::error("Framebuffer ({}) incomplete: 0x{:X}", tag, status);
        throw std::runtime_error(std::string("Framebuffer not complete: ") + tag);
    }
}

}

Framebuffer Framebuffer::depth_only(int width, int height) {
    Framebuffer f;
    f.kind_      = Kind::DepthOnly;
    f.has_depth_ = true;
    glGenFramebuffers(1, &f.fbo_);
    f.allocate_attachments(width, height);

    spdlog::info("Framebuffer (depth-only) {}x{} fbo={} depth_tex={}",
                 width, height, f.fbo_, f.depth_tex_);
    return f;
}

Framebuffer Framebuffer::hdr_color(int width, int height, bool with_depth) {
    Framebuffer f;
    f.kind_      = Kind::HdrColor;
    f.has_depth_ = with_depth;
    glGenFramebuffers(1, &f.fbo_);
    f.allocate_attachments(width, height);

    spdlog::info("Framebuffer (hdr-color{}) {}x{} fbo={} color_tex={} depth_tex={}",
                 with_depth ? "+depth" : "", width, height,
                 f.fbo_, f.color_tex_, f.depth_tex_);
    return f;
}

void Framebuffer::allocate_attachments(int width, int height) {
    if (width  <= 0) width  = 1;
    if (height <= 0) height = 1;
    size_ = {width, height};

    if (kind_ == Kind::DepthOnly) {
        glGenTextures(1, &depth_tex_);
        glBindTexture(GL_TEXTURE_2D, depth_tex_);
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

        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, depth_tex_, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        check_complete(fbo_, "depth-only");
        return;
    }

    // HDR color framebuffer
    glGenTextures(1, &color_tex_);
    glBindTexture(GL_TEXTURE_2D, color_tex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                 width, height, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (has_depth_) {
        glGenTextures(1, &depth_tex_);
        glBindTexture(GL_TEXTURE_2D, depth_tex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
                     width, height, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, color_tex_, 0);
    if (has_depth_) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, depth_tex_, 0);
    }
    constexpr GLenum kDrawBuf = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &kDrawBuf);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check_complete(fbo_, "hdr-color");
}

void Framebuffer::resize(int width, int height) {
    if (fbo_ == 0) return;
    if (width  <= 0) width  = 1;
    if (height <= 0) height = 1;
    if (size_.x == width && size_.y == height) return;

    release_attachments();
    allocate_attachments(width, height);
}

Framebuffer::~Framebuffer() {
    release();
}

Framebuffer::Framebuffer(Framebuffer&& o) noexcept
    : kind_     (o.kind_),
      has_depth_(o.has_depth_),
      fbo_      (std::exchange(o.fbo_,       0)),
      color_tex_(std::exchange(o.color_tex_, 0)),
      depth_tex_(std::exchange(o.depth_tex_, 0)),
      size_     (o.size_) {}

Framebuffer& Framebuffer::operator=(Framebuffer&& o) noexcept {
    if (this != &o) {
        release();
        kind_      = o.kind_;
        has_depth_ = o.has_depth_;
        fbo_       = std::exchange(o.fbo_,       0);
        color_tex_ = std::exchange(o.color_tex_, 0);
        depth_tex_ = std::exchange(o.depth_tex_, 0);
        size_      = o.size_;
    }
    return *this;
}

void Framebuffer::release_attachments() noexcept {
    if (color_tex_ != 0) { glDeleteTextures(1, &color_tex_); color_tex_ = 0; }
    if (depth_tex_ != 0) { glDeleteTextures(1, &depth_tex_); depth_tex_ = 0; }
}

void Framebuffer::release() noexcept {
    release_attachments();
    if (fbo_ != 0) { glDeleteFramebuffers(1, &fbo_); fbo_ = 0; }
    size_ = {0, 0};
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
