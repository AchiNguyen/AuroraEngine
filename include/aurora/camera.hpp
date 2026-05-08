#pragma once

#include <glm/glm.hpp>

namespace aurora {

class Camera {
public:
    enum class Direction { Forward, Backward, Left, Right, Up, Down };

    Camera(const glm::vec3& position, float aspect);

    glm::mat4 view_matrix() const;
    glm::mat4 projection_matrix(float aspect) const;

    void process_keyboard(Direction direction, float dt);
    void process_mouse(float dx, float dy);

    const glm::vec3& position() const { return position_; }
    const glm::vec3& front() const { return front_; }
    const glm::vec3& up() const { return up_; }
    const glm::vec3& right() const { return right_; }
    float yaw() const { return yaw_; }
    float pitch() const { return pitch_; }

private:
    void update_vectors();

    glm::vec3 position_;
    glm::vec3 front_{0.0f, 0.0f, -1.0f};
    glm::vec3 up_{0.0f, 1.0f, 0.0f};
    glm::vec3 right_{1.0f, 0.0f, 0.0f};
    glm::vec3 world_up_{0.0f, 1.0f, 0.0f};

    float yaw_ = -90.0f;
    float pitch_ = 0.0f;
    float movement_speed_ = 5.0f;
    float mouse_sensitivity_ = 0.1f;
    float aspect_ = 16.0f / 9.0f;
};

}
