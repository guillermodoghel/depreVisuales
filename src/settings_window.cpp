#include "settings_window.h"
#include "projectm_manager.h"
#include "audio_manager.h"
#include <imgui.h>
#include <algorithm>
#include <vector>
#include <string>

std::string currentPreset;  // Define the currentPreset variable
static std::vector<std::string> audioInputs;
static std::vector<int> deviceIndices;
static std::vector<std::string> filteredPresets;
static char searchBuffer[128] = "";
static bool shuffleEnabled = false;  // Shuffle state
static std::string currentAudioInput;  // Variable to store the current audio input name
static float gainValue = 1.0f;  // Variable to store the gain value

void InitializeSettings(const std::vector<std::string>& presetList, const std::vector<std::string>& audioInputList, const std::vector<int>& indices, bool shuffleState) {
    audioInputs = audioInputList;
    deviceIndices = indices;
    filteredPresets = presetList;  // Initially, no filter applied
    currentPreset = getCurrentPreset();  // Initialize the currentPreset
    shuffleEnabled = shuffleState;
    currentAudioInput = audioInputList.empty() ? "No audio input available" : audioInputList[0];  // Initialize the current audio input
}

std::string truncatePath(const std::string& path) {
    std::size_t pos = path.find("/presets-cream-of-the-crop/");
    if (pos != std::string::npos) {
        return path.substr(pos + 27);  // +27 to skip "/presets-cream-of-the-crop/"
    }
    return path;
}

void RenderSettingsWindow(bool& showSettingsWindow) {
    if (showSettingsWindow) {
        ImGui::Begin("Settings", &showSettingsWindow, ImGuiWindowFlags_NoCollapse);  // NoCollapse to prevent collapsing, but allow resizing and moving
        ImGui::SetWindowPos(ImVec2(100, 100), ImGuiCond_Once); // Position the window at (100, 100)

        // Fetch and display current preset
        currentPreset = getCurrentPreset();  // Update the currentPreset dynamically
        std::string truncatedPreset = truncatePath(currentPreset);
        ImGui::Text("Current Preset: %s", truncatedPreset.c_str());

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
        if (ImGui::BeginListBox("##preset_list", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
            for (size_t i = 0; i < filteredPresets.size(); ++i) {
                const bool isSelected = (filteredPresets[i] == currentPreset);
                std::string truncatedPreset = truncatePath(filteredPresets[i]);
                if (ImGui::Selectable(truncatedPreset.c_str(), isSelected)) {
                    setCurrentPreset(filteredPresets[i]);  // Set the selected preset as the current one
                    currentPreset = filteredPresets[i];  // Update the currentPreset
                }
            }
            ImGui::EndListBox();
        }

        // Add space between presets list and audio inputs list
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        // Display the current audio input
        ImGui::Text("Current Audio Input: %s", currentAudioInput.c_str());

        // Display audio inputs in a selectable list
        ImGui::Text("Audio Inputs:");
        if (ImGui::BeginListBox("##audio_input_list", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
            for (size_t i = 0; i < audioInputs.size(); ++i) {
                const bool isSelected = (audioInputs[i] == currentAudioInput);
                if (ImGui::Selectable(audioInputs[i].c_str(), isSelected)) {
                    currentAudioInput = audioInputs[i];  // Update the current audio input
                    setAudioInputDevice(deviceIndices[i]);  // Set the selected audio input device
                }
            }
            ImGui::EndListBox();
        }

        // Add space between audio inputs list and shuffle checkbox
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        // Display shuffle checkbox
        if (ImGui::Checkbox("Shuffle Enabled", &shuffleEnabled)) {
            setShuffleState(shuffleEnabled);
        }

        // Add space between shuffle checkbox and gain slider
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        // Display gain slider
        ImGui::Text("Gain:");
        if (ImGui::SliderFloat("##Gain", &gainValue, 0.0f, 1.0f)) {
            setGain(gainValue);
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::End();
    }
}
