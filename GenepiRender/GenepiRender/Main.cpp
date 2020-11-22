// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLEW

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
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

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include "render.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
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


const int xres = 1000;
const int yres = 1000;
const int tile_number = 8;
const int samples = 32;
const int nee_samples = 8;
int bounces[] = { 4, 3, 10 };
float variance_threshold = 0.001;


int main(int, char**)
{
    // setups parameters
    int xres = 1000;
    int yres = 1000;
    int tile_number = 8;
    int unified_samples = 8192;
    int nee_samples = 1;
    int gi_samples = 1;
    int samples[] = { unified_samples, nee_samples, gi_samples };
    int bounces[] = { 3, 3, 10 };
    float variance_threshold = 0.001;

    const char* filename = "D:/GenepiRender/Renders/pixar_kitchen.exr";
    const char* path = "D:/GenepiRender/Models/scene_drag.obj";

    Logger log(3);

    render_settings settings(xres, yres, samples, bounces, filename, log, tile_number);
    //camera cam(vec3(14.0f, 3.0f, 1.0f), vec3(-2.2f, 3.0f, -2.5f), 35, settings.xres, settings.yres, 0.075f, 14.0f); // pixar kitchen
    camera cam(vec3(0.0f, 7.5f, 30.0f), vec3(0.0f, 7.5f, 0.0f), 50, settings.xres, settings.yres, 0.0f, 20.0f, 1.0f, 1.0f);


    std::vector<light> lights;

    light square(2, true, 150.0f, vec3(1.0f), vec3(-5.0f, 15.0f, -5.0f), 10.0f, 10.0f, vec3(0, -1, 0));
    lights.push_back(square);

    //light square2(2, true, 300.0f, vec3(1.0f), vec3(-1.0f, 7.0f, -1.0f), 2.0f, 2.0f, vec3(0, 0, -1));
    //lights.push_back(square2);

    /*
    light dir_light(1, 15.0f, vec3(1.0f, 1.0f, 1.0f), vec3(0.6f, -0.5f, -0.6f), 10.0f);
    light dir_light2(1, 10.5f, vec3(1.0f, 0.45f, 0.07f), vec3(0.6f, -0.5f, -0.6f), 12.5f);

    light square_light(2, false, 150.0f, vec3(0.6f, 0.8f, 0.9f), vec3(-9.1f, 3.25f, -3.5f), 10.0f, 5.0f, vec3(1, 0, 0));
    light square_light2(2, false, 150.0f, vec3(0.6f, 0.8f, 0.9f), vec3(-9.1f, 3.25f, 3.0f), 10.0f, 5.0f, vec3(1, 0, 0));
    light square_bounce(2, false, 5.0f, vec3(1.0f, 0.45f, 0.07f), vec3(5.0f, 0.0f, -2.8f), 4.0f, 3.0f, vec3(0,0,1));
    light square_bounce2(2, false, 5.0f, vec3(1.0f, 0.45f, 0.07f), vec3(-2.0f, 0.0f, -2.8f), 4.0f, 3.0f, vec3(0, 0, 1));

    lights.push_back(square_light);
    lights.push_back(square_light2);
    //lights.push_back(square_bounce);
    //lights.push_back(square_bounce2);


    lights.push_back(dir_light);
    lights.push_back(dir_light2);
    */

    RTCDevice g_device = initializeDevice();
    RTCScene g_scene = rtcNewScene(g_device);

    rtcSetSceneBuildQuality(g_scene, RTC_BUILD_QUALITY_HIGH);
    rtcSetSceneFlags(g_scene, RTC_SCENE_FLAG_DYNAMIC | RTC_SCENE_FLAG_ROBUST);

    std::vector<material> materials;
    std::vector<light> lights2;

    load_scene(materials, lights, g_scene, g_device, path);
    rtcCommitScene(g_scene);

    //batch_render_omp(settings, cam, g_scene, materials, lights);

    color_t* pixels = (color_t*)malloc(xres * yres * sizeof(color_t));
    color_t* new_pixels = (color_t*)malloc(xres * yres * sizeof(color_t));

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Romano Render", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();


    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

    int s = 1;
    bool render = false;
    bool save_window = false;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        if (render)
        {
            progressive_render(s, pixels, settings, cam, g_scene, materials, lights, samples, bounces);

            for (int y = 0; y < yres; y++)
            {
                for (int x = 0; x < xres; x++)
                {
                    new_pixels[x + y * xres].R = pixels[x + y * xres].R / s;
                    new_pixels[x + y * xres].G = pixels[x + y * xres].G / s;
                    new_pixels[x + y * xres].B = pixels[x + y * xres].B / s;
                }
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, settings.xres, settings.yres, 0, GL_RGB, GL_FLOAT, new_pixels);
            s++;
        }
        
        
        {
            ImGui::Begin("RenderView");

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
                reset_render(pixels, new_pixels, settings.xres, settings.yres, s);
            }
            ImGui::SameLine();
            if (ImGui::Button("Save Image"))
            {
                save_window = true;
            }

            ImGui::Image((void*)image_texture, ImVec2(settings.xres, settings.yres));
            ImGui::End();
        }

        if (save_window)
        {
            ImGui::Begin("Save Image", &save_window);
            
            static char path[128] = "D:/image.png";

            ImGui::InputText("File Path", path, IM_ARRAYSIZE(path));

            if (ImGui::Button("Save"))
            {
                OIIO::ImageSpec spec(settings.xres, settings.yres, 3, OIIO::TypeDesc::FLOAT);
                OIIO::ImageBuf buffer(spec, new_pixels);

                //buffer = OIIO::ImageBufAlgo::flip(buffer);
                
                buffer.write(path);

                save_window = false;
            }

            ImGui::End();

        }

        
        {
            ImGui::Begin("Infos");                          
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Camera
        {
            ImGui::Begin("Camera");
            
            static float campos[3];
            static float camaim[3];

            static int focal_length = cam.focal_length;
            static float bokeh_power = cam.aperture;
            static float focus_distance = cam.focus_dist;
            static float anamorphic[2];
            static bool change = false;

            ImGui::InputFloat3("Camera Position", campos);
            ImGui::InputFloat3("Camera Aim", camaim);
            ImGui::InputInt("Focal Length", &focal_length, 0, 1000);
            ImGui::SliderFloat("Bokeh Power", &bokeh_power, 0, 10);
            ImGui::InputFloat("Focus Distance", &focus_distance, 0, 10000);
            ImGui::InputFloat2("Anamoprhic", anamorphic, 0.0f, 2.0f);
            if(ImGui::Button("Submit Changes")) change = true;

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

                reset_render(pixels, new_pixels, settings.xres, settings.yres, s);

                change = false;
            }

            ImGui::End();
        }

        // Light
        {
            ImGui::Begin("Light Editor");

            static int light_id;
            static float light_int;
            static float light_color[3];
            static float light_position[3];
            static float light_orientation[3];
            static float light_size[2];
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
                lights[light_id].size_x = light_size[0];
                lights[light_id].size_y = light_size[2];

                rtcCommitScene(g_scene);

                reset_render(pixels, new_pixels, settings.xres, settings.yres, s);

                change = false;
            }
        }

        // Materials
        {
            ImGui::Begin("Material Editor");

            static int mat_id;
            static float material_color[3];
            static float reflection_color[3];
            static float refraction_color[3];
            static float refrac;
            static float random_refrac;
            static float roughness;
            static float metallic;
            static float reflectance;
            static bool change = false;

            ImGui::InputInt("Material ID", &mat_id);
            ImGui::InputFloat3("Material Color", material_color);
            ImGui::InputFloat("Roughness", &roughness);
            ImGui::InputFloat("Reflectance", &reflectance);
            ImGui::InputFloat("Metallic", &metallic);
            ImGui::InputFloat3("Reflection Color", reflection_color);
            ImGui::InputFloat("Refraction", &refrac);
            ImGui::InputFloat("Refraction Fuzziness", &random_refrac);
            ImGui::InputFloat3("Refraction Color", refraction_color);
            if (ImGui::Button("Submit Changes")) change = true;

            if (change)
            {
                materials[mat_id].clr.x = material_color[0];
                materials[mat_id].clr.y = material_color[1];
                materials[mat_id].clr.z = material_color[2];

                materials[mat_id].roughness = roughness;
                materials[mat_id].reflectance = reflectance;
                materials[mat_id].metallic = metallic;

                materials[mat_id].reflection_color.x = reflection_color[0];
                materials[mat_id].reflection_color.y = reflection_color[1];
                materials[mat_id].reflection_color.z = reflection_color[2];

                materials[mat_id].refraction_color.x = refraction_color[0];
                materials[mat_id].refraction_color.y = refraction_color[1];
                materials[mat_id].refraction_color.z = refraction_color[2];

                materials[mat_id].refraction = refrac;
                materials[mat_id].refraction_roughness = random_refrac;

                reset_render(pixels, new_pixels, settings.xres, settings.yres, s);

                change = false;
            }
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
