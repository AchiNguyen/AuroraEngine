#include "aurora/shadow_pass.hpp"

#include "aurora/light.hpp"
#include "aurora/model.hpp"
#include "aurora/scene.hpp"
#include "aurora/transform.hpp"

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace aurora {

ShadowPass::ShadowPass(int map_size)
    : fbo_   (Framebuffer::depth_only(map_size, map_size)),
      shader_("shaders/shadow_depth.vert", "shaders/shadow_depth.frag") {}

glm::mat4 ShadowPass::light_space_matrix(const DirectionalLight& sun,
                                         glm::vec3 scene_center,
                                         float     scene_radius) {
    const glm::vec3 dir_norm = (glm::length(sun.direction) > 1e-5f)
        ? glm::normalize(sun.direction)
        : glm::vec3(0.0f, -1.0f, 0.0f);
    const glm::vec3 light_pos = scene_center - dir_norm * (scene_radius * 2.0f);

    glm::vec3 up{0.0f, 1.0f, 0.0f};
    if (std::abs(glm::dot(dir_norm, up)) > 0.99f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    const glm::mat4 view = glm::lookAt(light_pos, scene_center, up);
    const glm::mat4 proj = glm::ortho(-scene_radius, scene_radius,
                                      -scene_radius, scene_radius,
                                      0.1f, 4.0f * scene_radius);
    return proj * view;
}

void ShadowPass::render(const Scene& scene, const glm::mat4& light_space) {
    fbo_.bind();
    glViewport(0, 0, fbo_.size().x, fbo_.size().y);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    shader_.bind();
    shader_.set_uniform("u_light_space", light_space);

    for (const auto& node : scene.models) {
        if (!node.visible || !node.model) continue;
        shader_.set_uniform("u_model", node.transform.matrix());
        node.model->draw(shader_, /*depth_only=*/true);
    }

    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    fbo_.unbind();
}

}
