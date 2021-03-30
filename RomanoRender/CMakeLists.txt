# GLOBAL #

# Cmake 
cmake_minimum_required(VERSION 3.15)

# Defining VCPKG variable so that CMake can find the needed packages
# To work, this needs to be set before you set your project
set(CMAKE_TOOLCHAIN_FILE "D:/Romain/dev/vcpkg/vcpkg-master/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "CMAKE Toolchain" FORCE)
set(VCPKG_TARGET_TRIPLET "x86-windows" CACHE STRING "VCPKG Triplet" FORCE)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

# Project
project(RomanoRender)

# Add imgui dir
include_directories(Dependencies/glfw/include)
include_directories(imgui)
add_subdirectory(imgui)

# Add main directory
include_directories(src)
add_subdirectory(src)

