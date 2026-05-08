#include "aurora/transform.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>

namespace aurora {

glm::mat4 Transform::matrix() const {
    glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::rotate(m, glm::radians(rotation_euler.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::rotate(m, glm::radians(rotation_euler.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::rotate(m, glm::radians(rotation_euler.z), glm::vec3(0.0f, 0.0f, 1.0f));
    m = glm::scale(m, scale);
    return m;
}

glm::mat3 Transform::normal_matrix() const {
    return glm::mat3(glm::transpose(glm::inverse(matrix())));
}

}
