#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <vector>
#include <string>

void InitializeSettings(const std::vector<std::string>& presetList, const std::vector<std::string>& audioInputList, const std::vector<int>& deviceIndices, bool shuffleState = false);
void RenderSettingsWindow(bool& showSettingsWindow);
void setShuffleState(bool enabled); // Add this function declaration
void OpenURL(const char* url); // Add this function declaration
#endif // SETTINGS_WINDOW_H
