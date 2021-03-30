# SHADING #

set(shading_files
	light.h
	light.cpp
	material.h
	material.cpp
	bsdf.h
	bsdf.cpp
)

add_library(shading ${shading_files})

target_link_libraries(shading utils)