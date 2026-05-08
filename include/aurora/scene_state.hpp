#pragma once

#include "aurora/camera.hpp"
#include "aurora/light.hpp"
#include "aurora/material.hpp"

#include <glm/vec3.hpp>

namespace aurora {

struct SceneState {
    DirectionalLight& dir_light;
    PointLight&       point_light;
    Material&         material;
    Camera&           camera;

    float&     model_rotation_speed;
    bool&      model_rotate_enabled;
    glm::vec3& model_rotation_axis;
    glm::vec3& clear_color;

    bool&  dir_light_enabled;
    bool&  point_orbit_enabled;
    float& point_orbit_radius;
};

}
