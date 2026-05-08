#include "aurora/scene_renderer.hpp"

#include "aurora/camera.hpp"
#include "aurora/light.hpp"
#include "aurora/material.hpp"
#include "aurora/model.hpp"
#include "aurora/scene.hpp"
#include "aurora/shadow_pass.hpp"
#include "aurora/skybox.hpp"
#include "aurora/transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <string>
#include <utility>

namespace aurora {

SceneRenderer::SceneRenderer(std::shared_ptr<Shader> mesh_shader,
                             std::shared_ptr<Shader> lamp_shader,
                             Mesh                    lamp_mesh)
    : mesh_shader_(std::move(mesh_shader)),
      lamp_shader_(std::move(lamp_shader)),
      lamp_mesh_  (std::move(lamp_mesh)) {}

void SceneRenderer::run_shadow_pass(const Scene& scene) {
    if (!shadow_pass_ || !scene.shadows_enabled) return;
    const glm::mat4 light_space = ShadowPass::light_space_matrix(scene.dir_light,
                                                                 scene.scene_center,
                                                                 scene.scene_radius);
    shadow_pass_->render(scene, light_space);
}

void SceneRenderer::draw(const Scene& scene, const Camera& camera, glm::ivec2 viewport_size) {
    glm::mat4 light_space(1.0f);
    const bool render_shadows = shadow_pass_ && scene.shadows_enabled;
    if (render_shadows) {
        light_space = ShadowPass::light_space_matrix(scene.dir_light,
                                                     scene.scene_center,
                                                     scene.scene_radius);
    }

    const float aspect =
        static_cast<float>(viewport_size.x) /
        static_cast<float>(viewport_size.y > 0 ? viewport_size.y : 1);
    const glm::mat4 view       = camera.view_matrix();
    const glm::mat4 projection = camera.projection_matrix(aspect);

    Shader& shader = *mesh_shader_;
    shader.bind();
    shader.set_uniform("u_view",           view);
    shader.set_uniform("u_projection",     projection);
    shader.set_uniform("u_view_position",  camera.position());
    shader.set_uniform("u_ambient_factor", scene.ambient_factor);

    DirectionalLight effective_dir = scene.dir_light;
    effective_dir.enabled = scene.dir_light_enabled && effective_dir.enabled;
    upload(shader, "u_dir_light", effective_dir);

    int active_lights = 0;
    for (const auto& node : scene.point_lights) {
        if (active_lights >= kMaxPointLights) break;
        if (!node.visible) continue;
        upload(shader,
               "u_point_lights[" + std::to_string(active_lights) + "]",
               node.light);
        ++active_lights;
    }
    shader.set_uniform("u_num_point_lights", active_lights);

    if (render_shadows) {
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, shadow_pass_->depth_texture_id());
        shader.set_uniform("u_shadow_map",      3);
        shader.set_uniform("u_light_space",     light_space);
        shader.set_uniform("u_shadows_enabled", 1);
        shader.set_uniform("u_shadow_bias_min", scene.shadow_bias_min);
        shader.set_uniform("u_shadow_bias_max", scene.shadow_bias_max);
    } else {
        shader.set_uniform("u_shadows_enabled", 0);
    }

    for (const auto& node : scene.models) {
        if (!node.visible || !node.model) continue;
        shader.set_uniform("u_model", node.transform.matrix());
        node.model->draw(shader);
    }

    Shader& lamp = *lamp_shader_;
    lamp.bind();
    lamp.set_uniform("u_view",       view);
    lamp.set_uniform("u_projection", projection);
    for (const auto& node : scene.point_lights) {
        if (!node.visible) continue;
        const glm::mat4 lamp_model =
            glm::scale(glm::translate(glm::mat4(1.0f), node.light.position),
                       glm::vec3(0.1f));
        lamp.set_uniform("u_model", lamp_model);
        lamp.set_uniform("u_color", node.light.color * node.light.intensity);
        lamp_mesh_.draw();
    }

    if (skybox_ && scene.skybox_enabled) {
        skybox_->draw(view, projection, scene.skybox_brightness);
    }
}

void SceneRenderer::set_skybox(std::shared_ptr<Skybox> skybox) {
    skybox_ = std::move(skybox);
}

void SceneRenderer::set_shadow_pass(std::shared_ptr<ShadowPass> shadow_pass) {
    shadow_pass_ = std::move(shadow_pass);
}

}
