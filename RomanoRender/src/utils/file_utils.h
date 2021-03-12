#pragma once

#include <string>


// to use line()
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 
#endif


bool file_exists(const std::string& abs_filename);
std::string get_base_dir(const std::string& filepath);