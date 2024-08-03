#include "settings_window.h"
#include "projectm_manager.h"
#include <imgui.h>
#include <algorithm>
#include <vector>
#include <string>

static std::vector<std::string> audioInputs;
static std::vector<std::string> filteredPresets;
static char searchBuffer[128] = "";

void InitializeSettings(const std::vector<std::string>& presetList, const std::vector<std::string>& audioInputList) {
    audioInputs = audioInputList;
    filteredPresets = presetList;  // Initially, no filter applied
}

std::string truncatePath(const std::string& path) {
    std::size_t pos = path.find("/presets-cream-of-the-crop/");
    if (pos != std::string::npos) {
        return path.substr(pos + 27);  // +9 to skip "/display/"
    }
    return path;
}


void RenderSettingsWindow(bool& showSettingsWindow) {
    if (showSettingsWindow) {
        ImGui::Begin("Settings", &showSettingsWindow, ImGuiWindowFlags_NoCollapse);  // NoCollapse to prevent collapsing, but allow resizing and moving
        ImGui::SetWindowPos(ImVec2(100, 100), ImGuiCond_Once); // Position the window at (100, 100)

        // Search input for presets
        ImGui::Text("Search Presets");
        ImGui::SameLine();
        ImGui::InputText("##SearchPresets", searchBuffer, sizeof(searchBuffer));
        std::string searchString(searchBuffer);

        // Get the list of presets from projectm_manager
        const auto& presets = getPresetList();

        // Filter presets based on search input
        filteredPresets.clear();
        for (const auto& preset : presets) {
            if (preset.find(searchString) != std::string::npos) {
                filteredPresets.push_back(preset);
            }
        }

        // Display filtered presets in a selectable list
        ImGui::Text("Presets:");
        if (ImGui::BeginListBox("##preset_list")) {
            for (size_t i = 0; i < filteredPresets.size(); ++i) {
                const bool isSelected = false;
                std::string truncatedPreset = truncatePath(filteredPresets[i]);
                if (ImGui::Selectable(truncatedPreset.c_str(), isSelected)) {
                    // Handle selection (if needed)
                }
            }
            ImGui::EndListBox();
        }

        // Display audio inputs in a selectable list
        ImGui::Text("Audio Inputs:");
        if (ImGui::BeginListBox("##audio_input_list")) {
            for (size_t i = 0; i < audioInputs.size(); ++i) {
                const bool isSelected = false;
                if (ImGui::Selectable(audioInputs[i].c_str(), isSelected)) {
                    // Handle selection (if needed)
                }
            }
            ImGui::EndListBox();
        }

        ImGui::End();
    }
}
