#include "aurora/window.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>

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
        spec.title = "Aurora \xE2\x80\x94 Stage 1";
        aurora::Window window(spec);

        spdlog::info("Aurora started — entering render loop");

        while (!window.should_close()) {
            window.poll_events();

            glClearColor(kClearR, kClearG, kClearB, kClearA);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
