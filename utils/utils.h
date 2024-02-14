#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <termios.h>
#include <iomanip>
#include <filesystem>

const std::string kProjectPath = std::filesystem::current_path().parent_path().string() + "/";

std::string DateFormatting(int day);
std::string SpaceToPlus(std::string city_name);
char getch();
