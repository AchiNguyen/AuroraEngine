#pragma once

#include "aurora/light.hpp"
#include "aurora/transform.hpp"

#include <glm/vec3.hpp>

#include <memory>
#include <string>
#include <vector>

namespace aurora {

class Model;

struct ModelNode {
    std::string            name;
    std::shared_ptr<Model> model;
    Transform              transform;
    bool                   visible = true;
};

struct PointLightNode {
    std::string name;
    PointLight  light;
    Transform   transform;
    bool        visible      = true;
    bool        orbit        = false;
    float       orbit_radius = 3.0f;
    float       orbit_phase  = 0.0f;
};

class Scene {
public:
    std::vector<ModelNode>      models;
    std::vector<PointLightNode> point_lights;

    DirectionalLight dir_light{
        glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec3( 1.0f,  0.95f, 0.85f),
        0.5f,
    };
    bool      dir_light_enabled = true;
    glm::vec3 ambient_factor{0.1f};

    ModelNode&      add_model(std::string name, std::shared_ptr<Model> model, Transform transform = {});
    PointLightNode& add_point_light(std::string name, PointLight light, Transform transform = {});

    void update(float dt);
};

}
