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
    bool  dir_light_enabled = true;
    float ambient_factor    = 0.1f;

    bool  skybox_enabled    = true;
    float skybox_brightness = 1.0f;

    bool      shadows_enabled = true;
    int       shadow_map_size = 2048;
    float     shadow_bias_min = 0.0005f;
    float     shadow_bias_max = 0.005f;
    float     scene_radius    = 15.0f;
    glm::vec3 scene_center{0.0f};

    // HDR / post-processing parameters (Stage 12).
    float exposure          = 1.0f;
    float bloom_threshold   = 1.0f;
    float bloom_intensity   = 0.05f;
    float bloom_radius      = 1.5f;
    int   bloom_blur_passes = 5;
    float gamma             = 2.2f;
    bool  bloom_enabled     = true;

    ModelNode&      add_model(std::string name, std::shared_ptr<Model> model, Transform transform = {});
    PointLightNode& add_point_light(std::string name, PointLight light, Transform transform = {});

    void update(float dt);
};

}
