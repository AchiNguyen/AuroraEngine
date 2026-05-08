#include "aurora/scene_renderer.hpp"

#include "aurora/camera.hpp"
#include "aurora/light.hpp"
#include "aurora/material.hpp"
#include "aurora/model.hpp"
#include "aurora/scene.hpp"
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

void SceneRenderer::draw(const Scene& scene, const Camera& camera, glm::ivec2 viewport_size) {
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
    if (!scene.dir_light_enabled) {
        effective_dir.intensity = 0.0f;
    }
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

    for (const auto& node : scene.models) {
        if (!node.visible || !node.model) continue;
        const glm::mat4 model_mat    = node.transform.matrix();
        const glm::mat3 normal_mat   = node.transform.normal_matrix();
        shader.set_uniform("u_model",         model_mat);
        shader.set_uniform("u_normal_matrix", normal_mat);
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

}
