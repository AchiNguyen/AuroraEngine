#include "aurora/scene.hpp"

#include <cmath>
#include <utility>

namespace aurora {

ModelNode& Scene::add_model(std::string name, std::shared_ptr<Model> model, Transform transform) {
    ModelNode node;
    node.name      = std::move(name);
    node.model     = std::move(model);
    node.transform = transform;
    return models.emplace_back(std::move(node));
}

PointLightNode& Scene::add_point_light(std::string name, PointLight light, Transform transform) {
    PointLightNode node;
    node.name      = std::move(name);
    node.light     = light;
    node.transform = transform;
    node.transform.position = light.position;
    return point_lights.emplace_back(std::move(node));
}

void Scene::update(float dt) {
    for (auto& node : point_lights) {
        if (node.orbit) {
            node.orbit_phase += dt;
            node.transform.position = glm::vec3(
                std::cos(node.orbit_phase) * node.orbit_radius,
                node.transform.position.y,
                std::sin(node.orbit_phase) * node.orbit_radius
            );
        }
        node.light.position = node.transform.position;
    }
}

}
