#include "settings_window.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <string>

// Globals
std::vector<std::string> presets;
std::vector<std::string> audioInputs;
std::string selectedPreset;
std::string selectedAudioInput = "Input 1";

void InitializeSettings(std::vector<std::string>& presetList, std::vector<std::string>& audioInputList) {
    presets = presetList;
    audioInputs = audioInputList;
}

void ShowSettingsWindow() {
    static char searchBuffer[128] = "";
    ImGui::Begin("Settings");

    // Search bar
    ImGui::InputText("Search", searchBuffer, IM_ARRAYSIZE(searchBuffer));

    // List and select presets
    ImGui::Text("Presets");
    for (const auto& preset : presets) {
        if (strstr(preset.c_str(), searchBuffer)) {
            if (ImGui::Selectable(preset.c_str(), preset == selectedPreset)) {
                selectedPreset = preset;
                // Call function to load selected preset
            }
        }
    }

    // Dropdown for audio input
    ImGui::Text("Audio input");
    if (ImGui::BeginCombo("##combo", selectedAudioInput.c_str())) {
        for (const auto& input : audioInputs) {
            bool isSelected = (input == selectedAudioInput);
            if (ImGui::Selectable(input.c_str(), isSelected)) {
                selectedAudioInput = input;
                // Call function to switch audio input
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::End();
}
