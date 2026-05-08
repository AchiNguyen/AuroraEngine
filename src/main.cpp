#include "aurora/camera.hpp"
#include "aurora/cube.hpp"
#include "aurora/cubemap.hpp"
#include "aurora/debug_ui.hpp"
#include "aurora/light.hpp"
#include "aurora/model.hpp"
#include "aurora/plane.hpp"
#include "aurora/scene.hpp"
#include "aurora/scene_renderer.hpp"
#include "aurora/scene_state.hpp"
#include "aurora/shader.hpp"
#include "aurora/skybox.hpp"
#include "aurora/transform.hpp"
#include "aurora/window.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <array>
#include <cstdlib>
#include <exception>
#include <memory>
#include <string>
#include <utility>

int main() {
#ifdef AURORA_DEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif
    spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

    try {
        aurora::GlfwContext glfw;

        aurora::WindowSpec spec;
        spec.title = "Aurora \xE2\x80\x94 Stage 10";
        aurora::Window window(spec);

        glEnable(GL_DEPTH_TEST);

        const glm::ivec2 fb0 = window.framebuffer_size();
        const float initial_aspect =
            static_cast<float>(fb0.x) / static_cast<float>(fb0.y > 0 ? fb0.y : 1);

        aurora::Camera camera(glm::vec3{0.0f, 1.0f, 6.0f}, initial_aspect);

        auto backpack = std::make_shared<aurora::Model>(
            "assets/models/backpack/backpack.obj");

        aurora::Scene scene;
        scene.dir_light = aurora::DirectionalLight{
            glm::vec3(-0.2f, -1.0f, -0.3f),
            glm::vec3( 1.0f,  0.95f, 0.85f),
            0.5f,
        };
        scene.ambient_factor = 0.1f;

        {
            aurora::Transform t;
            scene.add_model("backpack_center", backpack, t);
        }
        {
            aurora::Transform t;
            t.position       = glm::vec3(-3.0f, 0.0f, -1.0f);
            t.rotation_euler = glm::vec3(0.0f,  30.0f, 0.0f);
            t.scale          = glm::vec3(0.8f);
            scene.add_model("backpack_left", backpack, t);
        }
        {
            aurora::Transform t;
            t.position       = glm::vec3( 3.0f, 0.0f, -1.0f);
            t.rotation_euler = glm::vec3(0.0f, -30.0f, 0.0f);
            t.scale          = glm::vec3(0.8f);
            scene.add_model("backpack_right", backpack, t);
        }
        {
            aurora::Material floor_mat;
            floor_mat.diffuse_map  = aurora::Texture::make_solid_color(glm::vec3(0.4f, 0.4f, 0.4f));
            floor_mat.specular_map = aurora::Texture::make_solid_color(glm::vec3(0.1f, 0.1f, 0.1f));
            floor_mat.shininess    = 32.0f;

            auto floor = std::make_shared<aurora::Model>(
                aurora::make_plane(1.0f, 1.0f),
                std::move(floor_mat));
            aurora::Transform t;
            t.position = glm::vec3(0.0f, -2.0f, 0.0f);
            t.scale    = glm::vec3(20.0f, 1.0f, 20.0f);
            scene.add_model("Floor", floor, t);
        }

        {
            aurora::PointLight pl{
                glm::vec3(4.0f, 1.5f, 0.0f),
                glm::vec3(0.4f, 0.6f, 1.0f),
                1.5f,
            };
            auto& node = scene.add_point_light("orbiter", pl);
            node.orbit        = true;
            node.orbit_radius = 4.0f;
        }
        {
            aurora::PointLight pl{
                glm::vec3(0.0f, 3.0f, 2.0f),
                glm::vec3(1.0f, 0.4f, 0.2f),
                2.0f,
            };
            scene.add_point_light("warm_top", pl);
        }

        aurora::SceneRenderer renderer(
            std::make_shared<aurora::Shader>("shaders/mesh.vert", "shaders/mesh.frag"),
            std::make_shared<aurora::Shader>("shaders/lamp.vert", "shaders/lamp.frag"),
            aurora::make_cube()
        );

        {
            auto cubemap = std::make_shared<aurora::Cubemap>(std::array<std::string, 6>{
                "assets/skybox/sci_fi/right.jpg",
                "assets/skybox/sci_fi/left.jpg",
                "assets/skybox/sci_fi/top.jpg",
                "assets/skybox/sci_fi/bottom.jpg",
                "assets/skybox/sci_fi/front.jpg",
                "assets/skybox/sci_fi/back.jpg",
            });
            renderer.set_skybox(std::make_shared<aurora::Skybox>(std::move(cubemap)));
        }

        glm::vec3 clear_color{0x0a / 255.0f, 0x0e / 255.0f, 0x27 / 255.0f};
        aurora::SceneState scene_state{ scene, camera, clear_color };

        aurora::DebugUI debug_ui(window.handle());

        spdlog::info("Aurora started \xE2\x80\x94 entering Stage 10 render loop");
        spdlog::info("Controls: WASD move, Space/LCtrl up-down, mouse look, TAB release cursor, ESC quit");

        double last_time     = glfwGetTime();
        double last_log_time = last_time;

        while (!window.should_close()) {
            window.poll_events();

            const double now = glfwGetTime();
            const float  dt  = static_cast<float>(now - last_time);
            last_time = now;

            scene.update(dt);

            debug_ui.record_frame(dt);
            debug_ui.begin_frame();

            const bool ui_kb    = debug_ui.want_capture_keyboard();
            const bool ui_mouse = debug_ui.want_capture_mouse();

            using Dir = aurora::Camera::Direction;
            if (!ui_kb) {
                if (window.is_key_pressed(GLFW_KEY_W))            camera.process_keyboard(Dir::Forward,  dt);
                if (window.is_key_pressed(GLFW_KEY_S))            camera.process_keyboard(Dir::Backward, dt);
                if (window.is_key_pressed(GLFW_KEY_A))            camera.process_keyboard(Dir::Left,     dt);
                if (window.is_key_pressed(GLFW_KEY_D))            camera.process_keyboard(Dir::Right,    dt);
                if (window.is_key_pressed(GLFW_KEY_SPACE))        camera.process_keyboard(Dir::Up,       dt);
                if (window.is_key_pressed(GLFW_KEY_LEFT_CONTROL)) camera.process_keyboard(Dir::Down,     dt);
            }

            const glm::vec2 mdelta = window.consume_mouse_delta();
            if (window.cursor_captured() && !ui_mouse) {
                camera.process_mouse(mdelta.x, mdelta.y);
            }

            const glm::ivec2 fb = window.framebuffer_size();

            glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderer.draw(scene, camera, fb);

            if (now - last_log_time >= 1.0) {
                spdlog::debug("Scene: {} models, {} point lights",
                              scene.models.size(), scene.point_lights.size());
                last_log_time = now;
            }

            debug_ui.render_panels(scene_state);
            debug_ui.end_frame();

            window.swap_buffers();
        }

        spdlog::info("Aurora shutting down cleanly");
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        spdlog::critical("Fatal: {}", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        spdlog::critical("Fatal: unknown exception");
        return EXIT_FAILURE;
    }
}
