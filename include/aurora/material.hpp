#pragma once

#include <glm/vec3.hpp>

#include <string>

namespace aurora {

class Shader;

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    // Canonical Phong material presets (Mark Kilgard / OpenGL teapot tables).
    static const Material brass;
    static const Material polished_silver;
    static const Material rubber_red;
    static const Material plastic_white;
};

void upload(Shader& shader, const std::string& name, const Material& material);

}
