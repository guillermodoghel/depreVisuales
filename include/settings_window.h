#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <vector>
#include <string>

void InitializeSettings(const std::vector<std::string>& presetList, const std::vector<std::string>& audioInputList, const std::vector<int>& deviceIndices);
void RenderSettingsWindow(bool& showSettingsWindow);

#endif // SETTINGS_WINDOW_H
