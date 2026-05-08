#pragma once

#include "aurora/texture.hpp"

#include <memory>
#include <string>

namespace aurora {

class Shader;

struct Material {
    std::shared_ptr<Texture> diffuse_map;
    std::shared_ptr<Texture> specular_map;
    std::shared_ptr<Texture> normal_map;
    float shininess            = 32.0f;
    float normal_map_intensity = 1.0f;
};

void upload(Shader& shader, const std::string& name, const Material& material);

}
