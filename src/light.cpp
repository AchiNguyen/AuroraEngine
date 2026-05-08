#include "aurora/light.hpp"

#include "aurora/shader.hpp"

namespace aurora {

void upload(Shader& shader, const std::string& name, const DirectionalLight& light) {
    shader.set_uniform(name + ".direction", light.direction);
    shader.set_uniform(name + ".color",     light.color);
    shader.set_uniform(name + ".intensity", light.intensity);
}

void upload(Shader& shader, const std::string& name, const PointLight& light) {
    shader.set_uniform(name + ".position",  light.position);
    shader.set_uniform(name + ".color",     light.color);
    shader.set_uniform(name + ".intensity", light.intensity);
    shader.set_uniform(name + ".constant",  light.constant);
    shader.set_uniform(name + ".linear",    light.linear);
    shader.set_uniform(name + ".quadratic", light.quadratic);
}

}
