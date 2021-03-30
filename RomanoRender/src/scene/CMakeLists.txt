# SCENE #

set(scene_files
	camera.h
	camera.cpp
	objloader.h
	scene.h
	scene.cpp
	settings.h
	settings.cpp
	stats.h
	stats.cpp
	tinyobjl.h
)

add_library(scene ${scene_files})