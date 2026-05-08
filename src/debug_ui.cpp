#include "aurora/debug_ui.hpp"

#include "aurora/camera.hpp"
#include "aurora/light.hpp"
#include "aurora/material.hpp"
#include "aurora/model.hpp"
#include "aurora/scene.hpp"
#include "aurora/scene_state.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

#include <cstddef>
#include <string>

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
    Scene&  scene  = state.scene;
    Camera& camera = state.camera;

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
        ImGui::Text("Models        : %d", static_cast<int>(scene.models.size()));
        ImGui::Text("Point lights  : %d", static_cast<int>(scene.point_lights.size()));
    }
    ImGui::End();

    if (ImGui::Begin("Camera")) {
        const glm::vec3& p = camera.position();
        ImGui::Text("Position : %.2f, %.2f, %.2f", p.x, p.y, p.z);
        ImGui::Text("Yaw      : %.2f", camera.yaw());
        ImGui::Text("Pitch    : %.2f", camera.pitch());
        ImGui::Separator();
        ImGui::SliderFloat("Move speed",        &camera.movement_speed(),    1.0f,  50.0f);
        ImGui::SliderFloat("Mouse sensitivity", &camera.mouse_sensitivity(), 0.01f, 1.0f);
        ImGui::SliderFloat("FOV",               &camera.fov(),               30.0f, 110.0f);
        if (ImGui::Button("Reset")) {
            camera.set_position({0.0f, 0.0f, 5.0f});
            camera.set_yaw_pitch(-90.0f, 0.0f);
        }
    }
    ImGui::End();

    if (ImGui::Begin("Directional Light")) {
        ImGui::Checkbox("Enabled", &scene.dir_light_enabled);
        glm::vec3 dir = scene.dir_light.direction;
        if (ImGui::SliderFloat3("Direction", &dir.x, -1.0f, 1.0f)) {
            const float len = glm::length(dir);
            scene.dir_light.direction =
                (len > 1e-5f) ? dir / len : glm::vec3(0.0f, -1.0f, 0.0f);
        }
        ImGui::ColorEdit3 ("Color",     &scene.dir_light.color.x);
        ImGui::SliderFloat("Intensity", &scene.dir_light.intensity, 0.0f, 5.0f);
    }
    ImGui::End();

    if (ImGui::Begin("Point Lights")) {
        if (scene.point_lights.empty()) {
            ImGui::TextDisabled("No point lights in scene.");
        }
        for (std::size_t i = 0; i < scene.point_lights.size(); ++i) {
            auto& node = scene.point_lights[i];
            ImGui::PushID(static_cast<int>(i));
            const std::string header =
                node.name.empty() ? ("Light " + std::to_string(i)) : node.name;
            if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Visible", &node.visible);
                ImGui::SameLine();
                ImGui::Checkbox("Orbit",   &node.orbit);
                if (node.orbit) {
                    ImGui::SliderFloat("Orbit radius", &node.orbit_radius, 0.5f, 10.0f);
                    ImGui::Text("Position : %.2f, %.2f, %.2f",
                                node.light.position.x,
                                node.light.position.y,
                                node.light.position.z);
                } else {
                    if (ImGui::SliderFloat3("Position",
                                            &node.transform.position.x,
                                            -10.0f, 10.0f)) {
                        node.light.position = node.transform.position;
                    }
                }
                ImGui::ColorEdit3 ("Color",     &node.light.color.x);
                ImGui::SliderFloat("Intensity", &node.light.intensity, 0.0f, 10.0f);
                ImGui::Separator();
                ImGui::SliderFloat("Constant",  &node.light.constant,  0.1f, 2.0f);
                ImGui::SliderFloat("Linear",    &node.light.linear,    0.0f, 1.0f, "%.4f");
                ImGui::SliderFloat("Quadratic", &node.light.quadratic, 0.0f, 0.5f, "%.4f");
            }
            ImGui::PopID();
        }
    }
    ImGui::End();

    if (ImGui::Begin("Models")) {
        if (scene.models.empty()) {
            ImGui::TextDisabled("No models in scene.");
        }
        for (std::size_t i = 0; i < scene.models.size(); ++i) {
            auto& node = scene.models[i];
            ImGui::PushID(static_cast<int>(i));
            const std::string header =
                node.name.empty() ? ("Model " + std::to_string(i)) : node.name;
            if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Visible", &node.visible);
                ImGui::SliderFloat3("Position", &node.transform.position.x,       -10.0f, 10.0f);
                ImGui::SliderFloat3("Rotation", &node.transform.rotation_euler.x, -180.0f, 180.0f);
                ImGui::SliderFloat3("Scale",    &node.transform.scale.x,           0.1f,   3.0f);
                if (node.model) {
                    Material& mat = node.model->primary_material();
                    ImGui::SliderFloat("Shininess",
                                       &mat.shininess,
                                       1.0f, 256.0f,
                                       "%.1f",
                                       ImGuiSliderFlags_Logarithmic);
                }
            }
            ImGui::PopID();
        }
    }
    ImGui::End();

    if (ImGui::Begin("Scene")) {
        ImGui::ColorEdit3("Clear color",    &state.clear_color.x);
        ImGui::ColorEdit3("Ambient factor", &scene.ambient_factor.x);
    }
    ImGui::End();
}

}
