#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <vector>
#include <string>
#include <imgui.h>

void InitializeSettings(const std::vector<std::string>& presetList, const std::vector<std::string>& audioInputList);
void RenderSettingsWindow(bool& showSettingsWindow);
void ScanPresets(const std::string& directory, std::vector<std::string>& presets);

#endif // SETTINGS_WINDOW_H
