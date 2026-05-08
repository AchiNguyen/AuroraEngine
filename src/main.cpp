#include "aurora/camera.hpp"
#include "aurora/cube.hpp"
#include "aurora/light.hpp"
#include "aurora/material.hpp"
#include "aurora/mesh.hpp"
#include "aurora/shader.hpp"
#include "aurora/texture.hpp"
#include "aurora/window.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#include <cmath>
#include <cstdlib>
#include <exception>
#include <memory>

namespace {

constexpr float kClearR = 0x0a / 255.0f;
constexpr float kClearG = 0x0e / 255.0f;
constexpr float kClearB = 0x27 / 255.0f;
constexpr float kClearA = 1.0f;

}

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
        spec.title = "Aurora \xE2\x80\x94 Stage 5";
        aurora::Window window(spec);

        glEnable(GL_DEPTH_TEST);

        const glm::ivec2 fb0 = window.framebuffer_size();
        const float initial_aspect =
            static_cast<float>(fb0.x) / static_cast<float>(fb0.y > 0 ? fb0.y : 1);

        aurora::Camera camera(glm::vec3{0.0f, 0.0f, 3.0f}, initial_aspect);
        aurora::Mesh   cube = aurora::make_cube();
        aurora::Mesh   lamp = aurora::make_cube();
        aurora::Shader shader     ("shaders/mesh.vert", "shaders/mesh.frag");
        aurora::Shader lamp_shader("shaders/lamp.vert", "shaders/lamp.frag");

        aurora::DirectionalLight dir_light{
            glm::vec3(-0.2f, -1.0f, -0.3f),
            glm::vec3( 1.0f,  0.95f, 0.85f),
            0.5f,
        };
        aurora::PointLight point_light{
            glm::vec3(2.0f, 2.0f, 2.0f),
            glm::vec3(0.4f, 0.6f, 1.0f),
            1.5f,
        };
        auto diffuse_map  = std::make_shared<aurora::Texture>(
            "assets/textures/container2.png", aurora::TextureType::Diffuse);
        auto specular_map = std::make_shared<aurora::Texture>(
            "assets/textures/container2_specular.png", aurora::TextureType::Specular);
        const aurora::Material crate{ diffuse_map, specular_map, 64.0f };

        spdlog::info("Aurora started \xE2\x80\x94 entering Stage 5 render loop");
        spdlog::info("Controls: WASD move, Space/LCtrl up-down, mouse look, TAB release cursor, ESC quit");

        double last_time     = glfwGetTime();
        double last_log_time = last_time;

        while (!window.should_close()) {
            window.poll_events();

            const double now = glfwGetTime();
            const float  dt  = static_cast<float>(now - last_time);
            last_time = now;

            using Dir = aurora::Camera::Direction;
            if (window.is_key_pressed(GLFW_KEY_W))            camera.process_keyboard(Dir::Forward,  dt);
            if (window.is_key_pressed(GLFW_KEY_S))            camera.process_keyboard(Dir::Backward, dt);
            if (window.is_key_pressed(GLFW_KEY_A))            camera.process_keyboard(Dir::Left,     dt);
            if (window.is_key_pressed(GLFW_KEY_D))            camera.process_keyboard(Dir::Right,    dt);
            if (window.is_key_pressed(GLFW_KEY_SPACE))        camera.process_keyboard(Dir::Up,       dt);
            if (window.is_key_pressed(GLFW_KEY_LEFT_CONTROL)) camera.process_keyboard(Dir::Down,     dt);

            const glm::vec2 mdelta = window.consume_mouse_delta();
            if (window.cursor_captured()) {
                camera.process_mouse(mdelta.x, mdelta.y);
            }

            const glm::ivec2 fb = window.framebuffer_size();
            const float aspect =
                static_cast<float>(fb.x) / static_cast<float>(fb.y > 0 ? fb.y : 1);

            const float t = static_cast<float>(now);

            // Orbit the point light in a horizontal circle around the cube.
            constexpr float kOrbitRadius = 3.0f;
            constexpr float kOrbitHeight = 1.5f;
            point_light.position = glm::vec3(
                std::cos(t) * kOrbitRadius,
                kOrbitHeight,
                std::sin(t) * kOrbitRadius
            );

            const glm::mat4 model =
                glm::rotate(glm::mat4(1.0f),
                            t * glm::radians(45.0f),
                            glm::vec3(0.5f, 1.0f, 0.0f));
            const glm::mat4 view       = camera.view_matrix();
            const glm::mat4 projection = camera.projection_matrix(aspect);

            glClearColor(kClearR, kClearG, kClearB, kClearA);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // --- Lit cube pass ---
            shader.bind();
            shader.set_uniform("u_model",         model);
            shader.set_uniform("u_view",          view);
            shader.set_uniform("u_projection",    projection);
            shader.set_uniform("u_view_position", camera.position());
            aurora::upload(shader, "u_material",    crate);
            aurora::upload(shader, "u_dir_light",   dir_light);
            aurora::upload(shader, "u_point_light", point_light);
            cube.draw();

            // --- Unlit lamp pass at the point light's position ---
            const glm::mat4 lamp_model =
                glm::scale(glm::translate(glm::mat4(1.0f), point_light.position),
                           glm::vec3(0.1f));
            lamp_shader.bind();
            lamp_shader.set_uniform("u_model",      lamp_model);
            lamp_shader.set_uniform("u_view",       view);
            lamp_shader.set_uniform("u_projection", projection);
            lamp_shader.set_uniform("u_color",      point_light.color * point_light.intensity);
            lamp.draw();

            if (now - last_log_time >= 1.0) {
                spdlog::debug("Point light at ({:.2f}, {:.2f}, {:.2f})",
                              point_light.position.x,
                              point_light.position.y,
                              point_light.position.z);
                last_log_time = now;
            }

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
