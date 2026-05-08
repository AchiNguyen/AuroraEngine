#include "aurora/material.hpp"

#include "aurora/shader.hpp"

namespace aurora {

void upload(Shader& shader, const std::string& name, const Material& material) {
    material.diffuse_map->bind(0);
    shader.set_uniform(name + ".diffuse", 0);

    material.specular_map->bind(1);
    shader.set_uniform(name + ".specular", 1);

    shader.set_uniform(name + ".shininess", material.shininess);
}

}
