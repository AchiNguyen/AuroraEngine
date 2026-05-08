#pragma once

#include "aurora/framebuffer.hpp"
#include "aurora/shader.hpp"

#include <glad/glad.h>
#include <glm/vec2.hpp>

namespace aurora {

struct PostParams {
    float exposure          = 1.0f;
    float bloom_threshold   = 1.0f;
    float bloom_soft_knee   = 0.5f;
    float bloom_intensity   = 0.05f;
    float bloom_radius      = 1.5f;
    int   bloom_blur_passes = 5;
    float gamma             = 2.2f;
    bool  bloom_enabled     = true;
};

class PostProcessor {
public:
    PostProcessor(int width, int height);
    ~PostProcessor();

    PostProcessor(const PostProcessor&) = delete;
    PostProcessor& operator=(const PostProcessor&) = delete;
    PostProcessor(PostProcessor&&) = delete;
    PostProcessor& operator=(PostProcessor&&) = delete;

    // Resize HDR FBO to (width,height); bloom buffers track at half resolution.
    void resize(int width, int height);

    // Bind HDR FBO, set viewport to its size, clear color+depth.
    void begin_scene_pass(float clear_r, float clear_g, float clear_b);

    // Run threshold + ping-pong blur + composite into the window framebuffer.
    void end_frame_to_window(int window_w, int window_h, const PostParams& params);

    GLuint     hdr_fbo_id()         const { return hdr_fbo_.fbo(); }
    GLuint     hdr_color_texture()  const { return hdr_fbo_.color_texture_id(); }
    glm::ivec2 hdr_size()           const { return hdr_fbo_.size(); }
    glm::ivec2 bloom_size()         const { return bloom_pingpong_[0].size(); }

private:
    void draw_fullscreen_triangle();

    Framebuffer hdr_fbo_;
    Framebuffer bloom_threshold_fbo_;
    Framebuffer bloom_pingpong_[2];

    Shader threshold_shader_;
    Shader blur_shader_;
    Shader composite_shader_;

    GLuint fullscreen_vao_ = 0;
};

}
