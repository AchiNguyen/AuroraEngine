#include "aurora/material.hpp"

#include "aurora/shader.hpp"

namespace aurora {

void upload(Shader& shader, const std::string& name, const Material& material) {
    const auto& diffuse  = material.diffuse_map  ? material.diffuse_map  : Texture::white_fallback();
    const auto& specular = material.specular_map ? material.specular_map : Texture::black_fallback();

    diffuse->bind(0);
    shader.set_uniform(name + ".diffuse", 0);

    specular->bind(1);
    shader.set_uniform(name + ".specular", 1);

    shader.set_uniform(name + ".shininess", material.shininess);
}

}
