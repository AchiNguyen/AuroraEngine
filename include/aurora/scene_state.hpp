#pragma once

#include <glm/vec3.hpp>

namespace aurora {

class Camera;
class PostProcessor;
class Scene;
class SceneRenderer;

struct SceneState {
    Scene&         scene;
    Camera&        camera;
    glm::vec3&     clear_color;
    SceneRenderer& renderer;
    PostProcessor* post = nullptr;
};

}
