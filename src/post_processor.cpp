#include "aurora/post_processor.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>

namespace aurora {

namespace {

constexpr int kBloomDivisor = 2;

int half_dim(int v) {
    return std::max(1, v / kBloomDivisor);
}

}

PostProcessor::PostProcessor(int width, int height)
    : hdr_fbo_            (Framebuffer::hdr_color(width, height, /*with_depth=*/true)),
      bloom_threshold_fbo_(Framebuffer::hdr_color(half_dim(width), half_dim(height), false)),
      bloom_pingpong_     {Framebuffer::hdr_color(half_dim(width), half_dim(height), false),
                           Framebuffer::hdr_color(half_dim(width), half_dim(height), false)},
      threshold_shader_   ("shaders/post/fullscreen.vert", "shaders/post/bloom_threshold.frag"),
      blur_shader_        ("shaders/post/fullscreen.vert", "shaders/post/blur.frag"),
      composite_shader_   ("shaders/post/fullscreen.vert", "shaders/post/composite.frag")
{
    glGenVertexArrays(1, &fullscreen_vao_);

    threshold_shader_.set_uniform("u_hdr_color",   0);
    blur_shader_     .set_uniform("u_input",       0);
    composite_shader_.set_uniform("u_hdr_color",   0);
    composite_shader_.set_uniform("u_bloom_color", 1);

    spdlog::info("PostProcessor initialized at {}x{} (bloom buffers {}x{})",
                 width, height, half_dim(width), half_dim(height));
}

PostProcessor::~PostProcessor() {
    if (fullscreen_vao_ != 0) {
        glDeleteVertexArrays(1, &fullscreen_vao_);
        fullscreen_vao_ = 0;
    }
}

void PostProcessor::resize(int width, int height) {
    if (width <= 0 || height <= 0) return;
    hdr_fbo_.resize(width, height);
    const int hw = half_dim(width);
    const int hh = half_dim(height);
    bloom_threshold_fbo_.resize(hw, hh);
    bloom_pingpong_[0]  .resize(hw, hh);
    bloom_pingpong_[1]  .resize(hw, hh);
}

void PostProcessor::begin_scene_pass(float clear_r, float clear_g, float clear_b) {
    hdr_fbo_.bind();
    glViewport(0, 0, hdr_fbo_.size().x, hdr_fbo_.size().y);
    glClearColor(clear_r, clear_g, clear_b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessor::draw_fullscreen_triangle() {
    glBindVertexArray(fullscreen_vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void PostProcessor::end_frame_to_window(int window_w, int window_h, const PostParams& params) {
    // Save pipeline state and configure for full-screen post passes.
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    const glm::ivec2 bloom_dim = bloom_pingpong_[0].size();

    GLuint final_bloom_tex = 0;

    if (params.bloom_enabled && params.bloom_blur_passes > 0) {
        // 1) Bloom threshold pass: read HDR color, write bright pixels to bloom_threshold_fbo.
        bloom_threshold_fbo_.bind();
        glViewport(0, 0, bloom_dim.x, bloom_dim.y);
        threshold_shader_.bind();
        threshold_shader_.set_uniform("u_threshold", params.bloom_threshold);
        threshold_shader_.set_uniform("u_soft_knee", params.bloom_soft_knee);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdr_fbo_.color_texture_id());
        draw_fullscreen_triangle();

        // 2) Separable Gaussian blur ping-pong.
        const int total_passes = params.bloom_blur_passes * 2;
        GLuint source_tex = bloom_threshold_fbo_.color_texture_id();
        int    write_idx  = 0;
        blur_shader_.bind();
        blur_shader_.set_uniform("u_radius", params.bloom_radius);
        for (int i = 0; i < total_passes; ++i) {
            bloom_pingpong_[write_idx].bind();
            glViewport(0, 0, bloom_dim.x, bloom_dim.y);
            const glm::vec2 dir = (i % 2 == 0) ? glm::vec2(1.0f, 0.0f)
                                               : glm::vec2(0.0f, 1.0f);
            blur_shader_.set_uniform("u_direction", dir);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, source_tex);
            draw_fullscreen_triangle();

            source_tex = bloom_pingpong_[write_idx].color_texture_id();
            write_idx  = 1 - write_idx;
        }
        final_bloom_tex = source_tex;
    } else {
        // Bloom disabled: clear one of the ping-pong buffers to black so the composite
        // still has a valid sampler binding.
        bloom_pingpong_[0].bind();
        glViewport(0, 0, bloom_dim.x, bloom_dim.y);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        final_bloom_tex = bloom_pingpong_[0].color_texture_id();
    }

    // 3) Composite pass: tone-map + bloom add + gamma into the window framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window_w, window_h);
    composite_shader_.bind();
    composite_shader_.set_uniform("u_exposure",        params.exposure);
    composite_shader_.set_uniform("u_bloom_intensity", params.bloom_enabled ? params.bloom_intensity : 0.0f);
    composite_shader_.set_uniform("u_gamma",           params.gamma);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdr_fbo_.color_texture_id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, final_bloom_tex);
    draw_fullscreen_triangle();

    // Restore for subsequent (e.g. ImGui) draws.
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_DEPTH_TEST);
}

}
