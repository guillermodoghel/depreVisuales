#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <vector>
#include <string>

void ShowSettingsWindow();
void InitializeSettings(std::vector<std::string>& presets, std::vector<std::string>& audioInputs);

#endif // SETTINGS_WINDOW_H
