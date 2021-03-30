# INTEGRATORS #

set(integrator_files
	pathtracer.h
	pathtracer.cpp
	cartoon.h
	cartoon.cpp
	ao.h
	ao.cpp
	scene_view.h
	scene_view.cpp
)

add_library(integrator_files integrators)

target_link_libraries(integrators shading)