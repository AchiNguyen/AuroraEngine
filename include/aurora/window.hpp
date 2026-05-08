#pragma once

#include <glm/vec2.hpp>

#include <string>

struct GLFWwindow;

namespace aurora {

class GlfwContext {
public:
    GlfwContext();
    ~GlfwContext();

    GlfwContext(const GlfwContext&) = delete;
    GlfwContext& operator=(const GlfwContext&) = delete;
    GlfwContext(GlfwContext&&) = delete;
    GlfwContext& operator=(GlfwContext&&) = delete;
};

struct WindowSpec {
    int width = 1280;
    int height = 720;
    std::string title = "Aurora";
    int gl_major = 4;
    int gl_minor = 6;
    int msaa_samples = 4;
};

class Window {
public:
    explicit Window(const WindowSpec& spec);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    bool should_close() const;
    void poll_events();
    void swap_buffers() const;

    GLFWwindow* handle() const { return window_; }
    int width() const { return spec_.width; }
    int height() const { return spec_.height; }
    glm::ivec2 framebuffer_size() const;

    bool is_key_pressed(int glfw_key) const;
    glm::vec2 consume_mouse_delta();
    void set_cursor_captured(bool captured);
    bool cursor_captured() const;

private:
    static void s_key_callback(GLFWwindow* w, int key, int scancode, int action, int mods);
    static void s_cursor_pos_callback(GLFWwindow* w, double x, double y);
    static void s_framebuffer_size_callback(GLFWwindow* w, int width, int height);

    void on_cursor_pos(double x, double y);
    void on_key(int key, int action);

    WindowSpec spec_;
    GLFWwindow* window_ = nullptr;

    bool first_mouse_ = true;
    double last_x_ = 0.0;
    double last_y_ = 0.0;
    double mouse_dx_ = 0.0;
    double mouse_dy_ = 0.0;
};

}
