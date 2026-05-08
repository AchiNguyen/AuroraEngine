#include "aurora/material.hpp"

#include "aurora/shader.hpp"

namespace aurora {

void upload(Shader& shader, const std::string& name, const Material& material) {
    const auto& diffuse  = material.diffuse_map  ? material.diffuse_map  : Texture::white_fallback();
    const auto& specular = material.specular_map ? material.specular_map : Texture::black_fallback();
    const auto& normal   = material.normal_map   ? material.normal_map   : Texture::flat_normal_fallback();

    diffuse->bind(0);
    shader.set_uniform(name + ".diffuse", 0);

    specular->bind(1);
    shader.set_uniform(name + ".specular", 1);

    normal->bind(2);
    shader.set_uniform(name + ".normal",         2);
    shader.set_uniform(name + ".has_normal_map", material.normal_map ? 1 : 0);

    shader.set_uniform(name + ".shininess", material.shininess);
    shader.set_uniform("u_normal_intensity", material.normal_map_intensity);
}

}
