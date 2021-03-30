# RENDER #

add_subdirectory(integrators)

set(render_files
	integrators.h
	integrators.cpp
)

add_library(render ${render_files})

target_link_libraries(render integrators)
