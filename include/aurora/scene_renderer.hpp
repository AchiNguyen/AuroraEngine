#pragma once

#include "aurora/mesh.hpp"
#include "aurora/shader.hpp"

#include <glm/vec2.hpp>

#include <memory>

namespace aurora {

class Camera;
class Scene;

class SceneRenderer {
public:
    static constexpr int kMaxPointLights = 8;

    SceneRenderer(std::shared_ptr<Shader> mesh_shader,
                  std::shared_ptr<Shader> lamp_shader,
                  Mesh                    lamp_mesh);

    SceneRenderer(const SceneRenderer&) = delete;
    SceneRenderer& operator=(const SceneRenderer&) = delete;
    SceneRenderer(SceneRenderer&&) noexcept = default;
    SceneRenderer& operator=(SceneRenderer&&) noexcept = default;

    void draw(const Scene& scene, const Camera& camera, glm::ivec2 viewport_size);

private:
    std::shared_ptr<Shader> mesh_shader_;
    std::shared_ptr<Shader> lamp_shader_;
    Mesh                    lamp_mesh_;
};

}
