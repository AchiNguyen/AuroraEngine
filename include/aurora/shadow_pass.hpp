#pragma once

#include "aurora/framebuffer.hpp"
#include "aurora/shader.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace aurora {

class Scene;
struct DirectionalLight;

class ShadowPass {
public:
    explicit ShadowPass(int map_size);

    static glm::mat4 light_space_matrix(const DirectionalLight& sun,
                                        glm::vec3 scene_center,
                                        float     scene_radius);

    void render(const Scene& scene, const glm::mat4& light_space);

    GLuint     depth_texture_id() const { return fbo_.depth_texture_id(); }
    glm::ivec2 size()             const { return fbo_.size(); }

private:
    Framebuffer fbo_;
    Shader      shader_;
};

}
