# SRC #

# CMake
cmake_minimum_required(VERSION 3.15)

# Project
project(RomanoRender)

#-------------------------------

# Find packages
find_package(embree 3 REQUIRED)
find_package(openimageio REQUIRED)
find_package(Boost 1.73.0 REQUIRED)
#find_package(alembic 1.7.16 REQUIRED)
find_package(OpenColorIO REQUIRED)

# Include dependencies
include_directories(${OpenImageIO_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OpenColorIO_INCLUDE_DIRS})

# Subdirectories
add_subdirectory(utils)
add_subdirectory(app)
add_subdirectory(shading)
add_subdirectory(scene)
add_subdirectory(render)

# Link dependencies libraries with own libraries
target_link_libraries(utils embree)
target_link_libraries(utils ${Boost_LIBRARIES})
target_link_libraries(utils ${OPENCOLORIO_LIBRARIES})

# Adding our executable
add_executable(${PROJECT_NAME} Main.cpp)

# Compiler features
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Link libraries
target_link_libraries(${PROJECT_NAME} embree)
target_link_libraries(${PROJECT_NAME} ${OPENIMAGEIO_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} Alembic::Alembic)
target_link_libraries(${PROJECT_NAME} ${OPENCOLORIO_LIBRARIES})

# Link RomanoRender libraries
target_link_libraries(${PROJECT_NAME} imgui)
target_link_libraries(${PROJECT_NAME} app)

# Copy utils to build directory
add_custom_command(
	TARGET  ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_SOURCE_DIR}/utils $<TARGET_FILE_DIR:${PROJECT_NAME}>
)