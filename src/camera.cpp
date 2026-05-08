#include "aurora/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

namespace aurora {

Camera::Camera(const glm::vec3& position, float aspect)
    : position_(position), aspect_(aspect) {
    update_vectors();
}

glm::mat4 Camera::view_matrix() const {
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::projection_matrix(float aspect) const {
    return glm::perspective(glm::radians(70.0f), aspect, 0.1f, 100.0f);
}

void Camera::process_keyboard(Direction direction, float dt) {
    const float v = movement_speed_ * dt;
    switch (direction) {
        case Direction::Forward:  position_ += front_ * v;    break;
        case Direction::Backward: position_ -= front_ * v;    break;
        case Direction::Right:    position_ += right_ * v;    break;
        case Direction::Left:     position_ -= right_ * v;    break;
        case Direction::Up:       position_ += world_up_ * v; break;
        case Direction::Down:     position_ -= world_up_ * v; break;
    }
}

void Camera::process_mouse(float dx, float dy) {
    yaw_   += dx * mouse_sensitivity_;
    pitch_ -= dy * mouse_sensitivity_;
    pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
    update_vectors();
}

void Camera::update_vectors() {
    const float y = glm::radians(yaw_);
    const float p = glm::radians(pitch_);

    glm::vec3 f;
    f.x = std::cos(y) * std::cos(p);
    f.y = std::sin(p);
    f.z = std::sin(y) * std::cos(p);

    front_ = glm::normalize(f);
    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_    = glm::normalize(glm::cross(right_, front_));
}

}
