#pragma once

#include <string>
#include <string_view>

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
    void poll_events() const;
    void swap_buffers() const;

    GLFWwindow* handle() const { return window_; }
    int width() const { return spec_.width; }
    int height() const { return spec_.height; }

private:
    WindowSpec spec_;
    GLFWwindow* window_ = nullptr;
};

}
