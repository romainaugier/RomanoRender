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


#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <chrono>
#include <future>
#include <algorithm>
#include <math.h>

#include "objloader.h"
#include "ray.h"
#include "triangle.h"
#include "camera.h"
#include "matrix.h"
#include "tiles.h"
#include "scene.h"
#include "material.h"
#include "light.h"
#include "utils.h"
#include "render.h"



int main(int, char**)
{
    int xres = 1280;
    int yres = 720;
    int tile_number = 8;
    int unified_samples = 32;
    int nee_samples = 1;
    int gi_samples = 1;
    int samples[] = { unified_samples, nee_samples, gi_samples };
    int bounces[] = { 6, 6, 10 };
    float variance_threshold = 0.001;

    const char* filename = "D:/GenepiRender/Renders/pixar_kitchen.exr";
    //const char* path = "D:/GenepiRender/Models/1964_shelby_cobra_daytona.obj";
    const char* path = "D:/GenepiRender/Models/sphere.obj";
    
    Logger log(3);
    stats render_stats(0);
    render_settings settings(xres, yres, samples, bounces, filename, log, tile_number);
    camera cam(vec3(0.0f, 7.5f, 30.0f), vec3(0.0f, 7.5f, 0.0f), 50, settings.xres, settings.yres, 0.0f, 20.0f, 1.0f, 1.0f);


    std::vector<light> lights;

    light square(2, true, 350.0f, vec3(1.0f), vec3(-2.5f, 15.0f, -2.5f), 5.0f, 5.0f, vec3(0, -1, 0));
    lights.push_back(square);

    light front(2, true, 400.0f, vec3(1.0f), vec3(-10.0f, 5.0f, 40.0f), 20.0f, 10.0f, vec3(0, 0, -1));
    //lights.push_back(front);

    light back(2, true, 400.0f, vec3(1.0f), vec3(-10.0f, 5.0f, -40.0f), 20.0f, 10.0f, vec3(0, 0, 1));
    //lights.push_back(back);

    //light square2(2, true, 350.0f, vec3(1.0f), vec3(30.0f, 7.5f, -5.0f), 10.0f, 10.0f, vec3(-1, 0, 0));
    //lights.push_back(square2);

    light dir(1, 10.0f, vec3(1.0f), vec3(1.0f, -1.0f, -0.6f), 1.0f);
    //lights.push_back(dir);
    //light dir2(1, 9.0f, vec3(1.0f, 0.6f, 0.2f), vec3(1.0f, -1.0f, -0.6f), 3.0f);
    //lights.push_back(dir2);

    light dome(3, 1.0f, "D:/Ressources/HDRIs/pond_2k.hdr");
    //lights.push_back(dome);

    light env(3, 1.0f, vec3(1.0f), vec3(0.0f));
    //lights.push_back(env);


    RTCDevice g_device = initializeDevice();
    RTCScene g_scene = rtcNewScene(g_device);

    rtcSetSceneBuildQuality(g_scene, RTC_BUILD_QUALITY_HIGH);
    rtcSetSceneFlags(g_scene, RTC_SCENE_FLAG_DYNAMIC | RTC_SCENE_FLAG_ROBUST);

    std::vector<material> materials;

    load_scene(materials, lights, g_scene, g_device, path, render_stats);
    rtcCommitScene(g_scene);

    //batch_render_omp(settings, cam, g_scene, materials, lights);

    color_t* pixels = (color_t*)malloc(xres * yres * sizeof(color_t));
    color_t* new_pixels = (color_t*)malloc(xres * yres * sizeof(color_t));


    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;


    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only


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

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // style
    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;

    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    //style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
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
    //style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    //style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    //style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    //style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    //style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
    //style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
    //style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);


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
    ImVec2 uv1((float)yres / 1000.0f, (float)xres / 1000.0f);
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

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        std::cout << render_stats.rays << "\n";

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
                uv1 = ImVec2((float)settings.yres / 1000.0f, (float)settings.xres / 1000.0f);

                cam.aspect = (float)settings.xres / (float)settings.yres;

                delete[] pixels;
                delete[] new_pixels;

                pixels = (color_t*)malloc(settings.xres * settings.yres * sizeof(color_t));
                new_pixels = (color_t*)malloc(settings.xres * settings.yres * sizeof(color_t));

                std::cout << settings.xres * settings.yres * sizeof(pixels) << "\n";

                glBindTexture(GL_TEXTURE_2D, image_texture);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, settings.xres, settings.yres, 0, GL_RGB, GL_FLOAT, new_pixels);

                glBindTexture(GL_TEXTURE_2D, 0);

                reset_render(pixels, new_pixels, settings.xres, settings.yres, s, y, render_avg);

                change = false;
            }

            ImGui::End();
        }

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // progressive render
        previous_y = y;


        if (render)
        {
            progressive_render(s, y, pixels, settings, cam, g_scene, materials, lights, samples, bounces, render_stats);

            for (int z = 0; z < settings.yres; z++)
            {
                for (int x = 0; x < settings.xres; x++)
                {
                    if (z <= y + 50)
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
                uv1.y = settings.yres / 1000;
                scrolling.x = 0.0f;
                scrolling.y = 0.0f;
                zoom = 1.0f;
            }
            ImGui::SameLine();
            if (ImGui::Button("Save Image"))
            {
                save_window = true;
            }

            ImVec2 size(1000, 950);

            ImVec2 origin(0.5f, 0.5f);

            const ImVec4 tint(1.0f, 1.0f, 1.0f, 1.0f);
            const ImVec4 border_color(0.0f, 0.0f, 0.0f, 1.0f);

            ImGui::SetScrollY(0.0f);
            ImGui::Image((void*)image_texture, size, uv0, uv1, tint, border_color);

            const bool is_hovered = ImGui::IsItemHovered();
            const bool is_active = ImGui::IsItemActive();

            if (is_hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
            {
                scrolling.x += io.MouseDelta.x * 0.001;
                scrolling.y += io.MouseDelta.y * 0.001;
            }

            const float mouse_wheel = io.MouseWheel;

            if (is_hovered && mouse_wheel != 0.0f)
            {
                uv0.x += (mouse_wheel * (0.05f * (float)settings.yres / 1000.0f));
                uv0.y += (mouse_wheel * (0.05f * (float)settings.xres / 1000.0f));
                uv1.x -= (mouse_wheel * (0.05f * (float)settings.yres / 1000.0f));
                uv1.y -= (mouse_wheel * (0.05f * (float)settings.xres / 1000.0f));
                zoom -= (mouse_wheel * 0.05f);
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

            static char path[256] = "D:/image.png";

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

                //buffer = OIIO::ImageBufAlgo::flip(buffer);

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

            ImGui::Text("Average spp is %f seconds", render_1spp_time);
            ImGui::Text("Rendered %i SPP", s);
            
            ImGui::End();
        }

        // camera window
        {
            ImGui::Begin("Camera");

            static float campos[3] = { cam.pos.x, cam.pos.y, cam.pos.z };
            static float camaim[3] = { cam.lookat.x, cam.lookat.y, cam.lookat.z };

            static int focal_length = cam.focal_length;
            static float bokeh_power = cam.aperture;
            static float focus_distance = cam.focus_dist;
            static float anamorphic[2] = { cam.anamorphic_x, cam.anamorphic_y };
            static bool change = false;

            ImGui::InputFloat3("Camera Position", campos);
            ImGui::InputFloat3("Camera Aim", camaim);
            ImGui::InputInt("Focal Length", &focal_length, 0, 1000);
            ImGui::SliderFloat("Bokeh Power", &bokeh_power, 0, 10);
            ImGui::InputFloat("Focus Distance", &focus_distance, 0, 10000);
            ImGui::InputFloat2("Anamoprhic", anamorphic, 0, 5);
            if (ImGui::Button("Submit Changes")) change = true;

            if (change)
            {
                cam.pos.x = campos[0];
                cam.pos.y = campos[1];
                cam.pos.z = campos[2];

                cam.lookat.x = camaim[0];
                cam.lookat.y = camaim[1];
                cam.lookat.z = camaim[2];

                cam.focal_length = focal_length;
                cam.aperture = bokeh_power;
                cam.focus_dist = focus_distance;

                cam.anamorphic_x = anamorphic[0];
                cam.anamorphic_y = anamorphic[1];

                reset_render(pixels, new_pixels, settings.xres, settings.yres, s, y, render_avg);

                change = false;
            }

            ImGui::End();
        }

        // light editor window
        {
            ImGui::Begin("Light Editor");

            static int light_id;
            static float light_int = lights[light_id].intensity;
            static float light_color[3] = { lights[light_id].color.x, lights[light_id].color.y, lights[light_id].color.z };
            static float light_position[3] = { lights[light_id].position.x, lights[light_id].position.y, lights[light_id].position.z };
            static float light_orientation[3] = { lights[light_id].orientation.x, lights[light_id].orientation.y, lights[light_id].orientation.z };
            static float light_size[2] = { lights[light_id].size_x, lights[light_id].size_y };
            static bool change = false;

            ImGui::InputInt("Light ID", &light_id);
            ImGui::InputFloat("Light Intensity", &light_int, 0, 1500);
            ImGui::InputFloat3("Light Color", light_color);
            ImGui::InputFloat3("Light Position", light_position);
            ImGui::InputFloat3("Light Orientation", light_orientation);
            ImGui::InputFloat2("Light Size", light_size);
            if (ImGui::Button("Submit Changes")) change = true;

            if (change)
            {
                lights[light_id].intensity = light_int;
                lights[light_id].color.x = light_color[0];
                lights[light_id].color.y = light_color[1];
                lights[light_id].color.z = light_color[2];
                lights[light_id].position.x = light_position[0];
                lights[light_id].position.y = light_position[1];
                lights[light_id].position.z = light_position[2];
                lights[light_id].orientation.x = light_orientation[0];
                lights[light_id].orientation.y = light_orientation[1];
                lights[light_id].orientation.z = light_orientation[2];
                lights[light_id].direction.x = light_orientation[0];
                lights[light_id].direction.y = light_orientation[1];
                lights[light_id].direction.z = light_orientation[2];
                lights[light_id].size_x = light_size[0];
                lights[light_id].size_y = light_size[2];

                rtcCommitScene(g_scene);

                reset_render(pixels, new_pixels, settings.xres, settings.yres, s, y, render_avg);

                change = false;
            }

            ImGui::End();
        }

        // material editor window
        {
            ImGui::Begin("Material Editor");

            static std::string mat_name;
            static float material_color[3];
            static float reflection_color[3];
            static float spec_color[3];
            static float refraction_color[3];
            static float diff_roughness;
            static float metallic;
            static float roughness;
            static float specular;
            static float reflectance;
            static float ior;
            static float refrac;
            static float rough_refrac;
            static bool change = false;
            
            ImGui::Text("%s", mat_name);

            if (ImGui::InputInt("Material ID", &mat_id))
            {
                mat_id = std::min(mat_id, (int)materials.size() - 1);
                mat_id = std::max(0, mat_id);

                mat_name = materials[mat_id].name;

                material_color[0] = materials[mat_id].clr.x;
                material_color[1] = materials[mat_id].clr.y;
                material_color[2] = materials[mat_id].clr.z;

                reflection_color[0] = materials[mat_id].reflection_color.x;
                reflection_color[1] = materials[mat_id].reflection_color.y;
                reflection_color[2] = materials[mat_id].reflection_color.z;

                refraction_color[0] = materials[mat_id].refraction_color.x;
                refraction_color[1] = materials[mat_id].refraction_color.y;
                refraction_color[2] = materials[mat_id].refraction_color.z;

                spec_color[0] = materials[mat_id].specular_color.x;
                spec_color[1] = materials[mat_id].specular_color.y;
                spec_color[2] = materials[mat_id].specular_color.z;

                diff_roughness = materials[mat_id].diffuse_roughness;
                metallic = materials[mat_id].metallic;
                roughness = materials[mat_id].roughness;
                specular = materials[mat_id].specular;
                reflectance = materials[mat_id].reflectance;
                ior = materials[mat_id].ior.x;
                refrac = materials[mat_id].refraction;
                rough_refrac = materials[mat_id].refraction_roughness;
            }
           

            
            //ImGui::InputFloat3("Diffuse Color", material_color);
            ImGui::ColorEdit3("Diffuse Color", material_color, ImGuiColorEditFlags_NoInputs);
            ImGui::InputFloat("Diffuse Roughness", &diff_roughness);
            ImGui::InputFloat("Reflectance", &reflectance);
            ImGui::ColorEdit3("Reflectance Color", reflection_color, ImGuiColorEditFlags_NoInputs);
            ImGui::InputFloat("Metallic", &metallic);
            ImGui::InputFloat("Specular", &specular);
            ImGui::InputFloat("Reflectance/Specular Roughness", &roughness);
            ImGui::ColorEdit3("Specular Color", spec_color, ImGuiColorEditFlags_NoInputs);
            ImGui::InputFloat("Ior", &ior);
            ImGui::InputFloat("Refraction", &refrac);
            ImGui::InputFloat("Refraction Roughness", &rough_refrac);
            ImGui::ColorEdit3("Refraction Color", refraction_color, ImGuiColorEditFlags_NoInputs);
            if (ImGui::Button("Submit Changes")) change = true;

            if (change)
            {
                materials[mat_id].clr.x = material_color[0];
                materials[mat_id].clr.y = material_color[1];
                materials[mat_id].clr.z = material_color[2];

                materials[mat_id].diffuse_roughness = diff_roughness;

                materials[mat_id].specular = specular;
                materials[mat_id].reflectance = reflectance;
                materials[mat_id].roughness = roughness;
                materials[mat_id].metallic = metallic;
                materials[mat_id].ior = vec3(ior);
                materials[mat_id].refraction = refrac;

                materials[mat_id].reflection_color.x = reflection_color[0];
                materials[mat_id].reflection_color.y = reflection_color[1];
                materials[mat_id].reflection_color.z = reflection_color[2];

                materials[mat_id].specular_color.x = spec_color[0];
                materials[mat_id].specular_color.y = spec_color[1];
                materials[mat_id].specular_color.z = spec_color[2];

                materials[mat_id].refraction_roughness = rough_refrac;

                materials[mat_id].refraction_color.x = refraction_color[0];
                materials[mat_id].refraction_color.y = refraction_color[1];
                materials[mat_id].refraction_color.z = refraction_color[2];


                reset_render(pixels, new_pixels, settings.xres, settings.yres, s, y, render_avg);

                change = false;
            }

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;

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
