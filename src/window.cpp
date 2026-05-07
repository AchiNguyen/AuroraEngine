#include "aurora/window.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <stdexcept>

namespace aurora {

namespace {

void glfw_error_callback(int error, const char* description) {
    spdlog::error("GLFW error {}: {}", error, description ? description : "(null)");
}

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height) {
    glViewport(0, 0, width, height);
}

void APIENTRY gl_debug_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei /*length*/,
    const GLchar* message,
    const void* /*userParam*/)
{
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
        return;
    }

    const char* source_str = "Other";
    switch (source) {
        case GL_DEBUG_SOURCE_API:             source_str = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_str = "WindowSystem"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "ShaderCompiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     source_str = "ThirdParty"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     source_str = "Application"; break;
        default: break;
    }

    const char* type_str = "Other";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               type_str = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "Deprecated"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_str = "Undefined"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         type_str = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         type_str = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              type_str = "Marker"; break;
        default: break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            spdlog::error("[GL][{}][{}][{}] {}", source_str, type_str, id, message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            spdlog::warn("[GL][{}][{}][{}] {}", source_str, type_str, id, message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            spdlog::info("[GL][{}][{}][{}] {}", source_str, type_str, id, message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        default:
            spdlog::debug("[GL][{}][{}][{}] {}", source_str, type_str, id, message);
            break;
    }
}

}

GlfwContext::GlfwContext() {
    glfwSetErrorCallback(glfw_error_callback);
    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
}

GlfwContext::~GlfwContext() {
    glfwTerminate();
}

Window::Window(const WindowSpec& spec) : spec_(spec) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, spec_.gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, spec_.gl_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, spec_.msaa_samples);
#ifdef AURORA_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    window_ = glfwCreateWindow(spec_.width, spec_.height, spec_.title.c_str(), nullptr, nullptr);
    if (!window_) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window_, key_callback);
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        throw std::runtime_error("Failed to load OpenGL function pointers via GLAD");
    }

    spdlog::info("OpenGL Version : {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    spdlog::info("GLSL Version   : {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    spdlog::info("Vendor         : {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    spdlog::info("Renderer       : {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    GLint flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_callback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        spdlog::info("OpenGL debug context enabled");
    }

    glEnable(GL_MULTISAMPLE);

    int fb_width = 0;
    int fb_height = 0;
    glfwGetFramebufferSize(window_, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);
}

Window::~Window() {
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
}

bool Window::should_close() const {
    return glfwWindowShouldClose(window_) == GLFW_TRUE;
}

void Window::poll_events() const {
    glfwPollEvents();
}

void Window::swap_buffers() const {
    glfwSwapBuffers(window_);
}

}
