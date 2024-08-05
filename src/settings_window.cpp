#include "settings_window.h"
#include "projectm_manager.h"
#include "audio_manager.h"
#include <imgui.h>
#include <vector>
#include <string>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <TargetConditionals.h>
#include <CoreFoundation/CoreFoundation.h>
#elif __linux__
#include <stdlib.h>
#endif

std::string currentPreset;
static std::vector<std::string> audioInputs;
static std::vector<int> deviceIndices;
static std::vector<std::string> filteredPresets;
static char searchBuffer[128] = "";
static bool shuffleEnabled = false;
static bool autoTransitionsEnabled = false;
static int transitionSeconds = 30; // Default value
static std::string currentAudioInput;
static float gainValue = 0.0f;
static float beatSensitivity = 1.0f; // Default value

void InitializeSettings(const std::vector<std::string>& presetList, const std::vector<std::string>& audioInputList, const std::vector<int>& indices, bool shuffleState) {
    audioInputs = audioInputList;
    deviceIndices = indices;
    filteredPresets = presetList;
    currentPreset = getCurrentPreset();
    shuffleEnabled = shuffleState;
    currentAudioInput = audioInputList.empty() ? "No audio input available" : audioInputList[0];
    beatSensitivity = projectm_get_beat_sensitivity(getProjectMHandle());
}

std::string truncatePath(const std::string& path) {
    std::size_t pos = path.find("/presets-cream-of-the-crop/");
    if (pos != std::string::npos) {
        return path.substr(pos + 27);
    }
    return path;
}

void RenderPresets() {
    ImGui::Text("Presets:");
    ImVec2 availableSpace = ImGui::GetContentRegionAvail();
    float presetListHeight = autoTransitionsEnabled ? availableSpace.y - 450.0f : availableSpace.y - 300.0f;
    if (ImGui::BeginListBox("##preset_list", ImVec2(-FLT_MIN, presetListHeight))) {
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
}

void RenderAudioInputs() {
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
}

void RenderTransitions() {
    if (ImGui::Checkbox("Auto Transitions", &autoTransitionsEnabled)) {
        updatePresetDuration(transitionSeconds, autoTransitionsEnabled);
    }
    if (autoTransitionsEnabled) {
        ImGui::Spacing();
        ImGui::Text("Seconds:");
        ImGui::SameLine();
        ImGui::PushItemWidth(120); // Adjusted width for at least 5 figures
        if (ImGui::InputInt("##TransitionSeconds", &transitionSeconds)) {
            updatePresetDuration(transitionSeconds, autoTransitionsEnabled);
        }
        ImGui::PopItemWidth();
    }
    ImGui::Spacing();
    if (ImGui::Checkbox("Shuffle Enabled", &shuffleEnabled)) {
        setShuffleState(shuffleEnabled);
    }
}

void RenderGainAndSensitivity() {
    ImGui::Text("Gain:");
    ImGui::PushItemWidth(-FLT_MIN);
    if (ImGui::SliderFloat("##Gain", &gainValue, -30.0f, 30.0f)) {
        setGain(gainValue);
    }
    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Beat Sensitivity:");
    ImGui::PushItemWidth(-FLT_MIN);
    if (ImGui::SliderFloat("##BeatSensitivity", &beatSensitivity, 0.0f, 2.0f)) {
        setBeatSensitivity(beatSensitivity);
    }
    ImGui::PopItemWidth();
}

void OpenURL(const char* url) {
#ifdef _WIN32
    ShellExecute(0, 0, url, 0, 0, SW_SHOW);
#elif __APPLE__
    std::string command = "open ";
    command += url;
    system(command.c_str());
#elif __linux__
    std::string command = "xdg-open ";
    command += url;
    system(command.c_str());
#endif
}

void RenderSettingsWindow(bool& showSettingsWindow) {
    if (showSettingsWindow) {
        ImGui::Begin("Depre settings", &showSettingsWindow, ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowPos(ImVec2(100, 100), ImGuiCond_Once);

        if (refreshPresets) {
            currentPreset = getCurrentPreset();
            refreshPresets = false;  // Reset the flag
        }

        std::string truncatedPreset = truncatePath(currentPreset);
        ImGui::Text("Current Preset: %s", truncatedPreset.c_str());

        ImGui::Text("Search Presets");
        ImGui::SameLine();
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

        RenderPresets();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        RenderAudioInputs();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        RenderTransitions();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        RenderGainAndSensitivity();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Add the label with the link
        ImGui::Text("Coded with manija - source: ");
        ImGui::SameLine();
        if (ImGui::SmallButton("https://github.com/guillermodoghel/depreVisuales")) {
            OpenURL("https://github.com/guillermodoghel/depreVisuales");
        }

        ImGui::End();
    }
}
