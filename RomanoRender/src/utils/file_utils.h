#pragma once

#include <string>


bool file_exists(const std::string& abs_filename);
std::string get_base_dir(const std::string& filepath);