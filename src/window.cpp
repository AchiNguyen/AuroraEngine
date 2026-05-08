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

    glfwSetWindowUserPointer(window_, this);

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window_, &Window::s_key_callback);
    glfwSetFramebufferSizeCallback(window_, &Window::s_framebuffer_size_callback);
    glfwSetCursorPosCallback(window_, &Window::s_cursor_pos_callback);

    if (glfwRawMouseMotionSupported() == GLFW_TRUE) {
        glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
        glfwSetWindowUserPointer(window_, nullptr);
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
}

bool Window::should_close() const {
    return glfwWindowShouldClose(window_) == GLFW_TRUE;
}

void Window::poll_events() {
    glfwPollEvents();
}

void Window::swap_buffers() const {
    glfwSwapBuffers(window_);
}

glm::ivec2 Window::framebuffer_size() const {
    int w = 0, h = 0;
    glfwGetFramebufferSize(window_, &w, &h);
    return {w, h};
}

bool Window::is_key_pressed(int glfw_key) const {
    return glfwGetKey(window_, glfw_key) == GLFW_PRESS;
}

glm::vec2 Window::consume_mouse_delta() {
    const glm::vec2 d{static_cast<float>(mouse_dx_), static_cast<float>(mouse_dy_)};
    mouse_dx_ = 0.0;
    mouse_dy_ = 0.0;
    return d;
}

void Window::set_cursor_captured(bool captured) {
    glfwSetInputMode(window_, GLFW_CURSOR,
                     captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    first_mouse_ = true;
    mouse_dx_ = 0.0;
    mouse_dy_ = 0.0;
    spdlog::debug("Cursor capture: {}", captured ? "ON" : "OFF");
}

bool Window::cursor_captured() const {
    return glfwGetInputMode(window_, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void Window::on_cursor_pos(double x, double y) {
    if (first_mouse_) {
        last_x_ = x;
        last_y_ = y;
        first_mouse_ = false;
        return;
    }
    mouse_dx_ += x - last_x_;
    mouse_dy_ += y - last_y_;
    last_x_ = x;
    last_y_ = y;
}

void Window::on_key(int key, int action) {
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    } else if (key == GLFW_KEY_TAB) {
        set_cursor_captured(!cursor_captured());
    }
}

void Window::s_key_callback(GLFWwindow* w, int key, int /*scancode*/, int action, int /*mods*/) {
    if (auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w))) {
        self->on_key(key, action);
    } else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(w, GLFW_TRUE);
    }
}

void Window::s_cursor_pos_callback(GLFWwindow* w, double x, double y) {
    if (auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w))) {
        self->on_cursor_pos(x, y);
    }
}

void Window::s_framebuffer_size_callback(GLFWwindow* /*w*/, int width, int height) {
    glViewport(0, 0, width, height);
}

}
