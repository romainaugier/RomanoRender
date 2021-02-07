cmake_minimum_required(VERSION 3.15)
project(RomanoRender)

set(CMAKE_TOOLCHAIN_FILE C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_TARGET_TRIPLET x86-windows)
set(CMAKE_VERBOSE_MAKEFILE ON)

#-------------------------------

find_package(embree 3.8.0 REQUIRED)
find_package(OpenImageIO REQUIRED)
find_package(Boost 1.73.0 REQUIRED)

#-------------------------------

set(source_files
	src/Main.cpp
	imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_impl_glfw.cpp
	imgui/imgui_impl_opengl3.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/imnodes.cpp)

include_directories(../Dependencies/glfw/include)
include_directories(../RomanoRender/imgui)
include_directories(../src)

#-------------------------------

include_directories(${OpenImageIO_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

#-------------------------------

add_executable(main src/Main.cpp)

#-------------------------------



#-------------------------------

target_link_libraries(main embree)
target_link_libraries(main OpenImageIO)
target_link_libraries(main ${Boost_LIBRARIES})