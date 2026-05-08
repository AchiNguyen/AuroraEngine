#pragma once

#include <glm/vec3.hpp>

#include <string>

namespace aurora {

class Shader;

struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 color;
    float     intensity;
    bool      enabled = true;
};

struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float constant  = 1.0f;
    float linear    = 0.09f;
    float quadratic = 0.032f;
};

void upload(Shader& shader, const std::string& name, const DirectionalLight& light);
void upload(Shader& shader, const std::string& name, const PointLight& light);

}
