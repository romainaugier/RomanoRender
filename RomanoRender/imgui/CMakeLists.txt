# IMGUI #

set(imgui_files
	gl3w.c
	ImFileDialog.h
	ImFileDialog.cpp
	imgui.h
	imgui.cpp
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_impl_glfw.h
	imgui_impl_glfw.cpp
	imgui_impl_opengl3.h
	imgui_impl_opengl3.cpp
	imgui_internal.h
	imgui_tables.cpp
	imgui_widgets.cpp
	imstb_rectpack.h
	imstb_textedit.h
	imstb_truetype.h
	stb_image.h
)

add_library(imgui ${imgui_files})

target_link_libraries(imgui ../../Dependencies/glfw/lib-vc2019/glfw3)