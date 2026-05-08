#pragma once

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace aurora {

struct Transform {
    glm::vec3 position{0.0f};
    glm::vec3 rotation_euler{0.0f};
    glm::vec3 scale{1.0f};

    glm::mat4 matrix() const;
    glm::mat3 normal_matrix() const;
};

}
