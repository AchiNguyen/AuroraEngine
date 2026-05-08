#include "aurora/debug_ui.hpp"

#include "aurora/camera.hpp"
#include "aurora/light.hpp"
#include "aurora/material.hpp"
#include "aurora/scene_state.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

namespace aurora {

DebugUI::DebugUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

DebugUI::~DebugUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DebugUI::begin_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void DebugUI::end_frame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool DebugUI::want_capture_mouse() const {
    return ImGui::GetIO().WantCaptureMouse;
}

bool DebugUI::want_capture_keyboard() const {
    return ImGui::GetIO().WantCaptureKeyboard;
}

void DebugUI::record_frame(float dt) {
    frame_history_[frame_history_idx_] = dt * 1000.0f;
    frame_history_idx_ = (frame_history_idx_ + 1) % kFrameHistory;

    sample_accum_ += dt;
    sample_count_++;
    if (sample_count_ >= 60) {
        const float avg_dt = sample_accum_ / static_cast<float>(sample_count_);
        ms_avg_  = avg_dt * 1000.0f;
        fps_avg_ = (avg_dt > 0.0f) ? (1.0f / avg_dt) : 0.0f;
        sample_accum_ = 0.0f;
        sample_count_ = 0;
    }
}

void DebugUI::render_panels(SceneState& state) {
    if (ImGui::Begin("Performance")) {
        ImGui::Text("FPS         : %.1f", fps_avg_);
        ImGui::Text("Frame time  : %.3f ms", ms_avg_);
        ImGui::PlotLines("Frame ms",
                         frame_history_,
                         kFrameHistory,
                         frame_history_idx_,
                         nullptr,
                         0.0f, 33.3f,
                         ImVec2(0.0f, 60.0f));
        if (ImGui::Checkbox("VSync", &vsync_enabled_)) {
            glfwSwapInterval(vsync_enabled_ ? 1 : 0);
        }
    }
    ImGui::End();

    if (ImGui::Begin("Camera")) {
        const glm::vec3& p = state.camera.position();
        ImGui::Text("Position : %.2f, %.2f, %.2f", p.x, p.y, p.z);
        ImGui::Text("Yaw      : %.2f", state.camera.yaw());
        ImGui::Text("Pitch    : %.2f", state.camera.pitch());
        ImGui::Separator();
        ImGui::SliderFloat("Move speed",      &state.camera.movement_speed(),    1.0f, 50.0f);
        ImGui::SliderFloat("Mouse sensitivity", &state.camera.mouse_sensitivity(), 0.01f, 1.0f);
        ImGui::SliderFloat("FOV",             &state.camera.fov(),               30.0f, 110.0f);
        if (ImGui::Button("Reset")) {
            state.camera.set_position({0.0f, 0.0f, 5.0f});
            state.camera.set_yaw_pitch(-90.0f, 0.0f);
        }
    }
    ImGui::End();

    if (ImGui::Begin("Directional Light")) {
        ImGui::Checkbox("Enabled", &state.dir_light_enabled);
        glm::vec3 dir = state.dir_light.direction;
        if (ImGui::SliderFloat3("Direction", &dir.x, -1.0f, 1.0f)) {
            const float len = glm::length(dir);
            state.dir_light.direction =
                (len > 1e-5f) ? dir / len : glm::vec3(0.0f, -1.0f, 0.0f);
        }
        ImGui::ColorEdit3 ("Color",     &state.dir_light.color.x);
        ImGui::SliderFloat("Intensity", &state.dir_light.intensity, 0.0f, 5.0f);
    }
    ImGui::End();

    if (ImGui::Begin("Point Light")) {
        ImGui::Checkbox("Orbit around origin", &state.point_orbit_enabled);
        if (state.point_orbit_enabled) {
            ImGui::SliderFloat("Orbit radius", &state.point_orbit_radius, 0.5f, 8.0f);
            ImGui::Text("Position : %.2f, %.2f, %.2f",
                        state.point_light.position.x,
                        state.point_light.position.y,
                        state.point_light.position.z);
        } else {
            ImGui::SliderFloat3("Position", &state.point_light.position.x, -10.0f, 10.0f);
        }
        ImGui::ColorEdit3 ("Color",     &state.point_light.color.x);
        ImGui::SliderFloat("Intensity", &state.point_light.intensity, 0.0f, 10.0f);
    }
    ImGui::End();

    if (ImGui::Begin("Material")) {
        ImGui::SliderFloat("Shininess",
                           &state.material.shininess,
                           1.0f, 256.0f,
                           "%.1f",
                           ImGuiSliderFlags_Logarithmic);
        ImGui::TextDisabled("Diffuse / specular maps are loaded from disk");
        ImGui::TextDisabled("and are not editable from this panel.");
    }
    ImGui::End();

    if (ImGui::Begin("Scene")) {
        ImGui::ColorEdit3("Clear color", &state.clear_color.x);
        ImGui::Separator();
        ImGui::Checkbox  ("Rotate model",          &state.model_rotate_enabled);
        ImGui::SliderFloat("Rotation speed (deg/s)", &state.model_rotation_speed, 0.0f, 180.0f);
        ImGui::SliderFloat3("Rotation axis",        &state.model_rotation_axis.x, -1.0f, 1.0f);
    }
    ImGui::End();
}

}
