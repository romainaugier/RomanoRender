cmake_minimum_required(VERSION 3.15)

# Defining VCPKG variable so that CMake can find the needed packages
set(CMAKE_TOOLCHAIN_FILE "D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "CMAKE Toolchain" FORCE)
set(VCPKG_TARGET_TRIPLET "x86-windows" CACHE STRING "VCPKG Triplet" FORCE)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(RomanoRender)

#-------------------------------

# Libraries dependencies
find_package(embree 3 REQUIRED)
find_package(OpenImageIO REQUIRED)
find_package(Boost 1.75.0 REQUIRED)

# Include dependencies
include_directories(${OpenImageIO_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

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

add_executable(${PROJECT_NAME} src/Main.cpp)

#-------------------------------



#-------------------------------

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE embree)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENIMAGEIO_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})

#-------------------------------

# Copy utils to build directory
add_custom_command(
	TARGET  ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_SOURCE_DIR}/utils $<TARGET_FILE_DIR:${PROJECT_NAME}>
)