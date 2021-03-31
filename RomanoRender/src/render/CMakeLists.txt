# RENDER #

add_subdirectory(integrators)

set(render_files
	render.h
	render.cpp
)

add_library(render ${render_files})

target_link_libraries(render integrators)
