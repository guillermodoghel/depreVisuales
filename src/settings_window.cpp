#include "settings_window.h"
#include "projectm_manager.h"
#include "audio_manager.h"
#include <imgui.h>
#include <vector>
#include <string>

std::string currentPreset;
static std::vector<std::string> audioInputs;
static std::vector<int> deviceIndices;
static std::vector<std::string> filteredPresets;
static char searchBuffer[128] = "";
static bool shuffleEnabled = false;
static std::string currentAudioInput;
static float gainValue = 1.0f;

void InitializeSettings(const std::vector<std::string>& presetList, const std::vector<std::string>& audioInputList, const std::vector<int>& indices, bool shuffleState) {
    audioInputs = audioInputList;
    deviceIndices = indices;
    filteredPresets = presetList;
    currentPreset = getCurrentPreset();
    shuffleEnabled = shuffleState;
    currentAudioInput = audioInputList.empty() ? "No audio input available" : audioInputList[0];
}

std::string truncatePath(const std::string& path) {
    std::size_t pos = path.find("/presets-cream-of-the-crop/");
    if (pos != std::string::npos) {
        return path.substr(pos + 27);
    }
    return path;
}

void RenderSettingsWindow(bool& showSettingsWindow) {
    if (showSettingsWindow) {
        ImGui::Begin("Settings", &showSettingsWindow, ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowPos(ImVec2(100, 100), ImGuiCond_Once);

        currentPreset = getCurrentPreset();
        std::string truncatedPreset = truncatePath(currentPreset);
        ImGui::Text("Current Preset: %s", truncatedPreset.c_str());

        ImGui::Text("Search Presets");
        ImGui::SameLine();
        // Make the search input field occupy the full width
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::InputText("##SearchPresets", searchBuffer, sizeof(searchBuffer));
        ImGui::PopItemWidth();
        std::string searchString(searchBuffer);

        const auto& presets = getPresetList();

        filteredPresets.clear();
        for (const auto& preset : presets) {
            if (preset.find(searchString) != std::string::npos) {
                filteredPresets.push_back(preset);
            }
        }

        ImGui::Text("Presets:");
        // Dynamically calculate the available space and set the height of the list box
        ImVec2 availableSpace = ImGui::GetContentRegionAvail();
        if (ImGui::BeginListBox("##preset_list", ImVec2(-FLT_MIN, availableSpace.y - 300.0f))) { // Adjust the -300.0f to fit other widgets
            for (size_t i = 0; i < filteredPresets.size(); ++i) {
                const bool isSelected = (filteredPresets[i] == currentPreset);
                std::string truncatedPreset = truncatePath(filteredPresets[i]);
                if (ImGui::Selectable(truncatedPreset.c_str(), isSelected)) {
                    setCurrentPreset(filteredPresets[i]);
                    currentPreset = filteredPresets[i];
                }
            }
            ImGui::EndListBox();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Current Audio Input: %s", currentAudioInput.c_str());

        ImGui::Text("Audio Inputs:");
        if (ImGui::BeginListBox("##audio_input_list", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
            for (size_t i = 0; i < audioInputs.size(); ++i) {
                const bool isSelected = (audioInputs[i] == currentAudioInput);
                if (ImGui::Selectable(audioInputs[i].c_str(), isSelected)) {
                    currentAudioInput = audioInputs[i];
                    setAudioInputDevice(deviceIndices[i]);
                }
            }
            ImGui::EndListBox();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Checkbox("Shuffle Enabled", &shuffleEnabled)) {
            setShuffleState(shuffleEnabled);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Gain:");
        // Make the slider occupy the full width
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::SliderFloat("##Gain", &gainValue, 0.0f, 1.0f)) {
            setGain(gainValue);
        }
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::End();
    }
}
