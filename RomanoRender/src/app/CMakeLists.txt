# APP #

set(app_files
	console.h
	console.cpp
	editor.h
	editor.cpp
	filedialog.h
	filedialog.cpp
	log.h
	log.cpp
	menubar.h
	menubar.cpp
	outliner.h
	outliner.cpp
	rendersettings.h
	rendersettings.cpp
	shelf.h
	shelf.cpp
)

add_library(app ${app_files})

target_link_libraries(app imgui)
target_link_libraries(app scene)
target_link_libraries(app render)