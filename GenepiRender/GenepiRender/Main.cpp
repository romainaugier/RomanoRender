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
    int xres = 1000;
    int yres = 1000;
    int tile_number = 8;
    int unified_samples = 32;
    int nee_samples = 1;
    int gi_samples = 1;
    int samples[] = { unified_samples, nee_samples, gi_samples };
    int bounces[] = { 3, 3, 10 };
    float variance_threshold = 0.001;

    const char* filename = "D:/GenepiRender/Renders/pixar_kitchen.exr";

    const char* path = "D:/GenepiRender/Models/mustang_clean_2.obj";

    
    Logger log(3);

    render_settings settings(xres, yres, samples, bounces, filename, log, tile_number);
    //camera cam(vec3(14.0f, 3.0f, 1.0f), vec3(-2.2f, 3.0f, -2.5f), 35, settings.xres, settings.yres, 0.075f, 14.0f); // pixar kitchen
    camera cam(vec3(0.0f, 7.5f, 30.0f), vec3(0.0f, 7.5f, 0.0f), 50, settings.xres, settings.yres, 0.0f, 20.0f, 1.0f, 1.0f);


    std::vector<light> lights;


    light square(2, true, 350.0f, vec3(1.0f), vec3(-10.0f, 25.0f, -10.0f), 20.0f, 20.0f, vec3(0, -1, 0));
    lights.push_back(square);


    light dome(3, 0.5f, vec3(0.2f, 0.6f, 0.75f), vec3(0.0f));
    lights.push_back(dome);


    RTCDevice g_device = initializeDevice();
    RTCScene g_scene = rtcNewScene(g_device);

    rtcSetSceneBuildQuality(g_scene, RTC_BUILD_QUALITY_HIGH);
    rtcSetSceneFlags(g_scene, RTC_SCENE_FLAG_DYNAMIC | RTC_SCENE_FLAG_ROBUST);

    std::vector<material> materials;

    load_scene(materials, lights, g_scene, g_device, path);
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

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    int s = 1;
    bool render = false;
    bool save_window = false;

    
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // progressive render
        auto start = get_time();

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
            glBindTexture(GL_TEXTURE_2D, image_texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, settings.xres, settings.yres, GL_RGB, GL_FLOAT, new_pixels);
            glBindTexture(GL_TEXTURE_2D, 0);
            s++;
        }

        auto end = get_time();
        std::chrono::duration<double> elapsed = end - start;

        double render_1spp_time = elapsed.count();

        // render view
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

        // save window
        if (save_window)
        {
            ImGui::Begin("Save Image", &save_window);

            static char path[256] = "D:/image.png";

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

        // info window
        {
            ImGui::Begin("Infos");

            ImGui::Text("Average per sample time is %f seconds", render_1spp_time);
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

                reset_render(pixels, new_pixels, settings.xres, settings.yres, s);

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

                reset_render(pixels, new_pixels, settings.xres, settings.yres, s);

                change = false;
            }
        }

        // material editor window
        {
            ImGui::Begin("Material Editor");

            static int mat_id;
            static float material_color[3] = { materials[mat_id].clr.x,  materials[mat_id].clr.y,  materials[mat_id].clr.z };
            static float reflection_color[3] = { materials[mat_id].reflection_color.x,  materials[mat_id].reflection_color.y,  materials[mat_id].reflection_color.z };
            static float refraction_color[3];
            static float diff_roughness = materials[mat_id].diffuse_roughness;
            static float metallic = materials[mat_id].metallic;
            static float roughness = materials[mat_id].roughness;
            static float specular = materials[mat_id].specular;
            static float ior = materials[mat_id].ior.x;
            static bool change = false;

            ImGui::InputInt("Material ID", &mat_id);
            ImGui::InputFloat3("Diffuse Color", material_color);
            ImGui::InputFloat("Diffuse Roughness", &diff_roughness);
            ImGui::InputFloat("Specular", &specular);
            ImGui::InputFloat("Specular Roughness", &roughness);
            ImGui::InputFloat("Metallic", &metallic);
            ImGui::InputFloat("Ior", &ior);
            ImGui::InputFloat3("Reflection Color", reflection_color);
            if (ImGui::Button("Submit Changes")) change = true;

            if (change)
            {
                materials[mat_id].clr.x = material_color[0];
                materials[mat_id].clr.y = material_color[1];
                materials[mat_id].clr.z = material_color[2];

                materials[mat_id].diffuse_roughness = diff_roughness;

                materials[mat_id].specular = specular;
                materials[mat_id].roughness = roughness;
                materials[mat_id].metallic = metallic;
                materials[mat_id].ior = vec3(ior);

                materials[mat_id].reflection_color.x = reflection_color[0];
                materials[mat_id].reflection_color.y = reflection_color[1];
                materials[mat_id].reflection_color.z = reflection_color[2];


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
