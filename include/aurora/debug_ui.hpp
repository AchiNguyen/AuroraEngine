#pragma once

struct GLFWwindow;

namespace aurora {

struct SceneState;

class DebugUI {
public:
    explicit DebugUI(GLFWwindow* window);
    ~DebugUI();

    DebugUI(const DebugUI&) = delete;
    DebugUI& operator=(const DebugUI&) = delete;
    DebugUI(DebugUI&&) = delete;
    DebugUI& operator=(DebugUI&&) = delete;

    void begin_frame();
    void end_frame();

    bool want_capture_mouse()    const;
    bool want_capture_keyboard() const;

    void record_frame(float dt);
    void render_panels(SceneState& state);

private:
    static constexpr int kFrameHistory = 240;

    float frame_history_[kFrameHistory] = {};
    int   frame_history_idx_ = 0;

    float fps_avg_ = 0.0f;
    float ms_avg_  = 0.0f;
    int   sample_count_ = 0;
    float sample_accum_ = 0.0f;

    bool vsync_enabled_ = true;
};

}
