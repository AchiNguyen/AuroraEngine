#include "aurora/material.hpp"

#include "aurora/shader.hpp"

namespace aurora {

const Material Material::brass = {
    {0.329412f, 0.223529f, 0.027451f},
    {0.780392f, 0.568627f, 0.113725f},
    {0.992157f, 0.941176f, 0.807843f},
    27.8974f, // 0.21794872 * 128
};

const Material Material::polished_silver = {
    {0.23125f,  0.23125f,  0.23125f},
    {0.2775f,   0.2775f,   0.2775f},
    {0.773911f, 0.773911f, 0.773911f},
    89.6f, // 0.7 * 128
};

const Material Material::rubber_red = {
    {0.05f, 0.0f,  0.0f},
    {0.5f,  0.4f,  0.4f},
    {0.7f,  0.04f, 0.04f},
    10.0f,
};

const Material Material::plastic_white = {
    {0.0f,  0.0f,  0.0f},
    {0.55f, 0.55f, 0.55f},
    {0.70f, 0.70f, 0.70f},
    32.0f,
};

void upload(Shader& shader, const std::string& name, const Material& material) {
    shader.set_uniform(name + ".ambient",   material.ambient);
    shader.set_uniform(name + ".diffuse",   material.diffuse);
    shader.set_uniform(name + ".specular",  material.specular);
    shader.set_uniform(name + ".shininess", material.shininess);
}

}
