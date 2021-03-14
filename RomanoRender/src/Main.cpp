#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#define IMGUI_IMPL_OPENGL_LOADER_GL3W

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


#include "OpenImageIO/imagebuf.h"

#include "render/integrators.h"
#include "app/log.h"
#include "app/console.h"
#include "app/outliner.h"
#include "app/menubar.h"
#include "app/shelf.h"
#include "scene/scene.h"
//#include "app/nodeeditor.h"
#include "Tracy.hpp"
#include "utils/utils.h"


int main(int, char**)
{
    // initializing rendering parameters
    int xres = 800;
    int yres = 800;
    int tile_number = 8;
    int unified_samples = 32;
    int nee_samples = 1;
    int gi_samples = 1;
    int samples[] = { unified_samples, nee_samples, gi_samples };
    int bounces[] = { 6, 6, 10, 1 };
    float variance_threshold = 0.001;

    // initializing different object we will need to render
    Logger log(3);
    Stats render_stats(0);
    Render_Settings settings(xres, yres, samples, bounces, log, tile_number);
    Camera initial_cam(vec3(0.0f, 7.5f, 30.0f), vec3(0.0f, 7.5f, 0.0f), 50, settings.xres, settings.yres, 0.0f, 20.0f, 1.0f, 1.0f);
    initial_cam.name = "Default Camera";
    
    // initializing entities containers
    std::vector<RTCGeometry> geometry;
    std::vector<Material> materials;
    std::vector<Light*> lights;
    std::vector<Camera> cameras;

    cameras.push_back(initial_cam);
    //lights.push_back(new Dome_Light(vec3(1.0f), 1.0f)); // this can be optional

    // loading sample sequences
    std::vector<std::vector<vec2>> sequence = load_sequences("D:/dev/Repos/Samples");

    int* pixel_ids = new int[settings.xres * settings.yres];
    
#pragma omp parallel for
    for (int i = 0; i < settings.xres * settings.yres - 1; i++)
    {
        pixel_ids[i] = (int)(generate_random_float_fast(i) * (sequence.size() - 1));
    }

    // initializing embree device and scene
    settings.device = initializeDevice();
    settings.scene = rtcNewScene(settings.device);

    rtcSetSceneBuildQuality(settings.scene, RTC_BUILD_QUALITY_HIGH);
    rtcSetSceneFlags(settings.scene, RTC_SCENE_FLAG_COMPACT | RTC_SCENE_FLAG_ROBUST);

    // initializing image buffers 
    color_t* pixels = (color_t*)malloc(xres * yres * sizeof(color_t));
    color_t* new_pixels = (color_t*)malloc(xres * yres * sizeof(color_t));

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;


    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);


    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Romano Render", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // setup Dear ImGui style
    ImGui::StyleColorsDark();

    // docking
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // style
    ImGuiStyle* style = &ImGui::GetStyle();

    // config
    style->WindowBorderSize = 0.0f;
    style->FrameBorderSize = 0.0f;
    style->ChildBorderSize = 0.0f;
    style->PopupBorderSize = 0.0f;
    style->TabBorderSize = 0.0f;
    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 0.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 0.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 0.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 0.0f;
    style->ChildRounding = 0.0f;
    style->TabRounding = 0.0f;
    style->PopupRounding = 0.0f;
    style->GrabRounding = 0.0f;
    style->LogSliderDeadzone = 0.0f;
    style->ScrollbarRounding = 0.0f;
    style->DisplaySafeAreaPadding = ImVec2(0.0f, 0.0f);
    style->WindowMenuButtonPosition = ImGuiDir_None;

    // colors
    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.04f);
    style->Colors[ImGuiCol_TabHovered] = ImVec4(0.718f, 0.718f, 0.718f, 0.6f);
    style->Colors[ImGuiCol_TabActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    style->Colors[ImGuiCol_DockingPreview] = ImVec4(1.0f, 0.45f, 0.0f, 1.0f);
    style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);


    // fonts
    ImFont* font_bold = io.Fonts->AddFontFromFileTTF("D://Fonts/OpenSans-Regular.ttf", 13.0f);
    ImFont* font_italic = io.Fonts->AddFontFromFileTTF("D://Fonts/OpenSans-Italic.ttf", 13.0f);
    ImFont* font_extrabolditalic = io.Fonts->AddFontFromFileTTF("D://Fonts/OpenSans-ExtraBoldItalic.ttf", 13.0f);
    ImFont* font_lightitalic = io.Fonts->AddFontFromFileTTF("D://Fonts/OpenSans-LightItalic.ttf", 13.0f);
    ImFont* font_bolditalic = io.Fonts->AddFontFromFileTTF("D://Fonts/OpenSans-BoldItalic.ttf", 13.0f);
    ImFont* font_semibolditalic = io.Fonts->AddFontFromFileTTF("D://Fonts/OpenSans-SemiboldItalic.ttf", 13.0f);
    ImFont* font_light = io.Fonts->AddFontFromFileTTF("D://Fonts/OpenSans-Light.ttf", 13.0f);
    ImFont* font_regular = io.Fonts->AddFontFromFileTTF("D://Fonts/OpenSans-Bold.ttf", 13.0f); 
    ImFont* font_semibold = io.Fonts->AddFontFromFileTTF("D://Fonts/OpenSans-Semibold.ttf", 13.0f);
    ImFont* font_extrabold = io.Fonts->AddFontFromFileTTF("D://Fonts/OpenSans-ExtraBold.ttf", 13.0f);


    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

    int s = 1;
    int y = 0;

    int mat_id = 0;

    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
    double render_1spp_time = 0.0;
    double render_avg = 0.0;

    int previous_y = 0;
    bool render = false;
    bool save_window = false;

    ImVec2 uv0(0, 0);
    ImVec2 uv1((float)yres / 500.0f, (float)xres / 1000.0f);
    float zoom = 1.0f;
    
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, settings.xres, settings.yres, 0, GL_RGB, GL_FLOAT, new_pixels);
   
    glBindTexture(GL_TEXTURE_2D, 0);

    // declaring all the editors
    Console console;
    Outliner outliner;
    MenuBar menubar;
    Geometry_Shelf geo_shelf;
    Light_Shelf light_shelf;
    Camera_Shelf cam_shelf;

    // initializing the file dialog texture methods
    file_dialog_init();

    bool edited = false;
    int change = 0;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // draw the different windows

        // main menu bar
        menubar.draw();

        // geometry shelf
        geo_shelf.draw(settings, geometry, materials, lights, cameras, console);

        // light shelf
        light_shelf.draw(settings, geometry, materials, lights, cameras, console);

        // cameras shelf
        cam_shelf.draw(settings, geometry, materials, lights, cameras, console);

        if (change > 0 || edited)
        {
            reset_render(pixels, new_pixels, settings.xres, settings.yres, s, y, render_avg);
            //change = 0;
            edited = false;
        }


        // Render Settings Editor
        {
            ImGui::Begin("Render Settings");

            static bool change = false;
            static int resolution[2];

            ImGui::InputInt2("Resolution", resolution);
            if (ImGui::Button("Submit Changes")) change = true;

            if (change)
            {
                settings.xres = resolution[0];
                settings.yres = resolution[1];

                uv0 = ImVec2(0, 0);
                uv1 = ImVec2((float)settings.yres / 500.0f, (float)settings.xres / 1000.0f);

                cameras[0].aspect = (float)settings.xres / (float)settings.yres;

                delete[] pixels;
                delete[] new_pixels;
                delete[] pixel_ids;

                pixel_ids = new int[settings.xres * settings.yres];

#pragma omp parallel for
                for (int i = 0; i < settings.xres * settings.yres - 1; i++)
                {
                    pixel_ids[i] = (int)(generate_random_float_fast(i) * (sequence.size() - 1));
                }

                pixels = (color_t*)malloc(settings.xres * settings.yres * sizeof(color_t));
                new_pixels = (color_t*)malloc(settings.xres * settings.yres * sizeof(color_t));

                glBindTexture(GL_TEXTURE_2D, image_texture);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, settings.xres, settings.yres, 0, GL_RGB, GL_FLOAT, new_pixels);

                glBindTexture(GL_TEXTURE_2D, 0);

                reset_render(pixels, new_pixels, settings.xres, settings.yres, s, y, render_avg);

                change = false;
            }

            ImGui::End();
        }


        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        // progressive render
        previous_y = y;


        if (render && s > 1)
        {
            progressive_render(s, pixel_ids, y, sequence, pixels, settings, cameras[0], materials, lights, samples, bounces, render_stats);

#pragma omp parallel for
            for (int z = 0; z < settings.yres; z++)
            {
                for (int x = 0; x < settings.xres; x++)
                {
                    if (z <= y + 50)
                    {
                        new_pixels[x + z * settings.xres].R = powf(pixels[x + z * settings.xres].R / s, 0.45f);
                        new_pixels[x + z * settings.xres].G = powf(pixels[x + z * settings.xres].G / s, 0.45f);
                        new_pixels[x + z * settings.xres].B = powf(pixels[x + z * settings.xres].B / s, 0.45f);

                        /*
                        pixels[x + y * settings.xres].R += powf((double)col.x, 0.45);
                        pixels[x + y * settings.xres].G += powf((double)col.y, 0.45);
                        pixels[x + y * settings.xres].B += powf((double)col.z, 0.45);
                        */
                    }
                }
            }

            glBindTexture(GL_TEXTURE_2D, image_texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, settings.xres, settings.yres, GL_RGB, GL_FLOAT, new_pixels);
            glBindTexture(GL_TEXTURE_2D, 0);

            if (y == 0)
            {
                start = get_time();
            }

            if (y < settings.yres - 50)
            {
                y += 50;
                //y = std::min(y, settings.yres);
            }

            if (y == settings.yres)
            {
                s++;

                //std::cout << y << "\n";

                y = 0;


                end = get_time();
                std::chrono::duration<double> elapsed = end - start;
                render_avg += elapsed.count();
            }

            if (y == previous_y) y = settings.yres;
        }


        if (render && s < 2)
        {
            progressive_render_fast(s, pixel_ids, sequence, pixels, settings, cameras[0], materials, lights, samples, bounces, render_stats);

#pragma omp parallel for
            for (int z = 0; z < settings.yres; z++)
            {
                for (int x = 0; x < settings.xres; x++)
                {
                    new_pixels[x + z * settings.xres].R = pixels[x + z * settings.xres].R / s;
                    new_pixels[x + z * settings.xres].G = pixels[x + z * settings.xres].G / s;
                    new_pixels[x + z * settings.xres].B = pixels[x + z * settings.xres].B / s;

                    /*
                    pixels[x + y * settings.xres].R += powf((double)col.x, 0.45);
                    pixels[x + y * settings.xres].G += powf((double)col.y, 0.45);
                    pixels[x + y * settings.xres].B += powf((double)col.z, 0.45);
                    */
                    
                }
            }

            glBindTexture(GL_TEXTURE_2D, image_texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, settings.xres, settings.yres, GL_RGB, GL_FLOAT, new_pixels);
            glBindTexture(GL_TEXTURE_2D, 0);

            s++;
        }


        // render view
        {
            ImGui::Begin("RenderView");

            ImVec2 scrolling(0.0f, 0.0f);

            if (ImGui::Button("Start"))
            {
                render = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Stop"))
            {
                render = false;
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                reset_render(pixels, new_pixels, settings.xres, settings.yres, s, y, render_avg);
                uv0.x = 0.0f;
                uv0.y = 0.0f;
                uv1.x = settings.xres / 1000;
                uv1.y = settings.yres / 500;
                scrolling.x = 0.0f;
                scrolling.y = 0.0f;
                zoom = 1.0f;
            }

            ImGui::SameLine();

            if (ImGui::Button("Save Image"))
            {
                save_window = true;
            }

            ImVec2 size(1000, 500);

            ImVec2 origin(0.5f, 0.5f);

            const ImVec4 tint(1.0f, 1.0f, 1.0f, 1.0f);
            const ImVec4 border_color(0.0f, 0.0f, 0.0f, 1.0f);

            ImGui::SetScrollY(0.0f);
            ImGui::Image((void*)image_texture, size, uv0, uv1, tint, border_color);

            const bool is_hovered = ImGui::IsItemHovered();
            const bool is_active = ImGui::IsItemActive();

            if (is_hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
            {
                scrolling.x += io.MouseDelta.x * 0.001;
                scrolling.y += io.MouseDelta.y * 0.001;
            }

            const float mouse_wheel = io.MouseWheel;

            if (is_hovered && mouse_wheel != 0.0f)
            {
                uv0.x += (mouse_wheel * (0.03f * (float)settings.yres / 500.0f));
                uv0.y += (mouse_wheel * (0.03f * (float)settings.xres / 1000.0f));
                uv1.x -= (mouse_wheel * (0.03f * (float)settings.yres / 500.0f));
                uv1.y -= (mouse_wheel * (0.03f * (float)settings.xres / 1000.0f));
                zoom -= (mouse_wheel * 0.03f);
            }

            float z = (zoom > 0.1f) ? zoom : 0.1;

            uv0.x -= (scrolling.x * z);
            uv1.x -= (scrolling.x * z);
            uv0.y -= (scrolling.y * z);
            uv1.y -= (scrolling.y * z);

            ImGui::End();
        }

        // save window
        if (save_window)
        {
            ImGui::Begin("Save Image", &save_window);

            static char path[512] = "D:/image.exr";

            ImGui::InputText("File Path", path, IM_ARRAYSIZE(path));

            if (ImGui::Button("Save"))
            {
                color_t* out_pixels = (color_t*)malloc(settings.xres * settings.yres * sizeof(color_t));

                for (int z = 0; z < settings.yres; z++)
                {
                    for (int x = 0; x < settings.xres; x++)
                    {
                        out_pixels[x + z * settings.xres].R = pixels[x + z * settings.xres].R / s;
                        out_pixels[x + z * settings.xres].G = pixels[x + z * settings.xres].G / s;
                        out_pixels[x + z * settings.xres].B = pixels[x + z * settings.xres].B / s;    
                    }
                }

                OIIO::ImageSpec spec(settings.xres, settings.yres, 3, OIIO::TypeDesc::FLOAT);
                OIIO::ImageBuf buffer(spec, out_pixels);

                buffer.write(path);

                delete[] out_pixels;

                save_window = false;
            }

            ImGui::End();

        }

        // info window
        {
            ImGui::Begin("Infos");

            render_1spp_time = render_avg / s;

            ImGui::Text("Rendered %i SPP", s);
            
            ImGui::End();
        }

        // console
        bool o = true;
        console.Draw("Console", &o);

        // outliner
        outliner.draw(cameras, lights, console);

        // Rendering
        ImGui::Render();

        
        int display_w, display_h;
        change--;

        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
