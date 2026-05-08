#include "aurora/camera.hpp"
#include "aurora/cube.hpp"
#include "aurora/debug_ui.hpp"
#include "aurora/light.hpp"
#include "aurora/mesh.hpp"
#include "aurora/model.hpp"
#include "aurora/scene_state.hpp"
#include "aurora/shader.hpp"
#include "aurora/window.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#include <cmath>
#include <cstdlib>
#include <exception>

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
        spec.title = "Aurora \xE2\x80\x94 Stage 7";
        aurora::Window window(spec);

        glEnable(GL_DEPTH_TEST);

        const glm::ivec2 fb0 = window.framebuffer_size();
        const float initial_aspect =
            static_cast<float>(fb0.x) / static_cast<float>(fb0.y > 0 ? fb0.y : 1);

        aurora::Camera camera(glm::vec3{0.0f, 0.0f, 5.0f}, initial_aspect);
        aurora::Mesh   lamp = aurora::make_cube();
        aurora::Shader shader     ("shaders/mesh.vert", "shaders/mesh.frag");
        aurora::Shader lamp_shader("shaders/lamp.vert", "shaders/lamp.frag");

        aurora::Model model("assets/models/backpack/backpack.obj");

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

        bool      dir_light_enabled    = true;
        bool      point_orbit_enabled  = true;
        float     point_orbit_radius   = 3.0f;
        bool      model_rotate_enabled = true;
        float     model_rotation_speed = 20.0f;
        glm::vec3 model_rotation_axis  {0.0f, 1.0f, 0.0f};
        glm::vec3 clear_color {0x0a / 255.0f, 0x0e / 255.0f, 0x27 / 255.0f};

        aurora::SceneState scene_state{
            dir_light,
            point_light,
            model.primary_material(),
            camera,
            model_rotation_speed,
            model_rotate_enabled,
            model_rotation_axis,
            clear_color,
            dir_light_enabled,
            point_orbit_enabled,
            point_orbit_radius,
        };

        aurora::DebugUI debug_ui(window.handle());

        spdlog::info("Aurora started \xE2\x80\x94 entering Stage 7 render loop");
        spdlog::info("Controls: WASD move, Space/LCtrl up-down, mouse look, TAB release cursor, ESC quit");

        constexpr float kOrbitHeight = 1.5f;
        float orbit_phase = 0.0f;
        float model_angle_deg = 0.0f;

        double last_time     = glfwGetTime();
        double last_log_time = last_time;

        while (!window.should_close()) {
            window.poll_events();

            const double now = glfwGetTime();
            const float  dt  = static_cast<float>(now - last_time);
            last_time = now;

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
            const float aspect =
                static_cast<float>(fb.x) / static_cast<float>(fb.y > 0 ? fb.y : 1);

            if (point_orbit_enabled) {
                orbit_phase += dt;
                point_light.position = glm::vec3(
                    std::cos(orbit_phase) * point_orbit_radius,
                    kOrbitHeight,
                    std::sin(orbit_phase) * point_orbit_radius
                );
            }

            if (model_rotate_enabled) {
                model_angle_deg += model_rotation_speed * dt;
            }
            const float axis_len = glm::length(model_rotation_axis);
            const glm::vec3 axis = (axis_len > 1e-5f)
                ? model_rotation_axis / axis_len
                : glm::vec3(0.0f, 1.0f, 0.0f);

            const glm::mat4 model_mat =
                glm::rotate(glm::mat4(1.0f),
                            glm::radians(model_angle_deg),
                            axis);
            const glm::mat4 view       = camera.view_matrix();
            const glm::mat4 projection = camera.projection_matrix(aspect);

            glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            aurora::DirectionalLight effective_dir = dir_light;
            if (!dir_light_enabled) effective_dir.intensity = 0.0f;

            shader.bind();
            shader.set_uniform("u_model",         model_mat);
            shader.set_uniform("u_view",          view);
            shader.set_uniform("u_projection",    projection);
            shader.set_uniform("u_view_position", camera.position());
            aurora::upload(shader, "u_dir_light",   effective_dir);
            aurora::upload(shader, "u_point_light", point_light);
            model.draw(shader);

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
