# UTILS

set(utils_files
	vec3.h
	vec2.h
	utils.h
	ray.h
	matrix.h
	maths_utils.h
	maths_utils.cpp
	file_utils.h
	file_utils.cpp
	embree_utils.h
	embree_utils.cpp
	ocio_utils.h
	ocio_utils.cpp
	sampling_utils.h
	sampling_utils.cpp
	str_utils.h
	str_utils.cpp
	windows_utils.h
	windows_utils.cpp
)

add_library(utils ${utils_files})