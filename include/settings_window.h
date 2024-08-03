#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <vector>
#include <string>

void InitializeSettings(const std::vector<std::string>& presetList, const std::vector<std::string>& audioInputList);
void RenderSettingsWindow(bool& showSettingsWindow);
extern std::string currentPreset;  // Declare the currentPreset variable

#endif // SETTINGS_WINDOW_H
