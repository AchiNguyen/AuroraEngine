#pragma once

#include "aurora/mesh.hpp"
#include "aurora/shader.hpp"

#include <glm/vec2.hpp>

#include <memory>

namespace aurora {

class Camera;
class Scene;
class ShadowPass;
class Skybox;

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

    // Runs the shadow pass into the shadow FBO. Leaves no specific framebuffer
    // bound by contract; the caller must bind the desired target FBO before
    // calling draw(). Safe to call when shadows are disabled (no-op).
    void run_shadow_pass(const Scene& scene);

    // Renders the scene (models, lamps, skybox) into whichever framebuffer is
    // currently bound by the caller. Does not touch framebuffer bindings.
    void draw(const Scene& scene, const Camera& camera, glm::ivec2 viewport_size);

    void                    set_skybox(std::shared_ptr<Skybox> skybox);
    std::shared_ptr<Skybox> skybox() const { return skybox_; }

    void                        set_shadow_pass(std::shared_ptr<ShadowPass> shadow_pass);
    std::shared_ptr<ShadowPass> shadow_pass() const { return shadow_pass_; }

private:
    std::shared_ptr<Shader>     mesh_shader_;
    std::shared_ptr<Shader>     lamp_shader_;
    Mesh                        lamp_mesh_;
    std::shared_ptr<Skybox>     skybox_;
    std::shared_ptr<ShadowPass> shadow_pass_;
};

}
