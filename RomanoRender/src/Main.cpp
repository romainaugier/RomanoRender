#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include <GL/gl3w.h>            // Initialize with gl3wInit()

#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


#include "OpenImageIO/imagebuf.h"

#include "render/render.h"
#include "app/log.h"
#include "app/console.h"
#include "app/outliner.h"
#include "app/menubar.h"
#include "app/shelf.h"
#include "app/editor.h"
#include "app/renderview.h"
#include "app/rendersettings.h"
#include "scene/scene.h"
#include "utils/utils.h"
#include "utils/ocio_utils.h"


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
    int bounces[] = { 10, 10, 10, 1 };
    float variance_threshold = 0.001;

    // initializing the ocio context
    OCIO::ConstConfigRcPtr ocio_config = initialize_ocio_config();
    OCIO_Utils ocio_utils(ocio_config);

    // initializing different object we will need to render
    Logger log(3);
    Stats render_stats(0);
    Render_Settings settings(xres, yres, samples, bounces, log, tile_number);
    Camera initial_cam(vec3(0.0f, 7.5f, 30.0f), vec3(0.0f, 7.5f, 0.0f), 50, settings.xres, settings.yres, 0.0f, 20.0f, 1.0f, 1.0f);
    initial_cam.name = "Default Camera";
    
    // initializing entities containers
    std::vector<Object> objects;
    std::vector<Material> materials;
    std::vector<Light*> lights;
    std::vector<Camera> cameras;

    cameras.push_back(initial_cam); // this can be optional, but it will be the Default Camera
    initial_cam.~Camera();

    // loading sample sequences
    std::vector<std::vector<vec2>> sequence = load_sequences("D:/GenepiRender/Samples");

    int* pixel_ids =  (int*)malloc(settings.xres * settings.yres * sizeof(int));
    
#pragma omp parallel for
    for (int i = 0; i < settings.xres * settings.yres; i++)
    {
        pixel_ids[i] = (int)(generate_random_float_fast(i) * (sequence.size() - 2));
    }

    // initializing embree device and scene
    settings.device = initializeDevice();
    settings.scene = rtcNewScene(settings.device);

    rtcSetSceneBuildQuality(settings.scene, RTC_BUILD_QUALITY_HIGH);
    rtcSetSceneFlags(settings.scene, RTC_SCENE_FLAG_COMPACT | RTC_SCENE_FLAG_ROBUST);

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
    bool err = gl3wInit() != 0;

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
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // style
    ImGuiStyle* style = &ImGui::GetStyle();

    // config
    style->WindowBorderSize = 0.0f;
    style->FrameBorderSize = 0.0f;
    style->ChildBorderSize = 0.0f;
    style->PopupBorderSize = 0.0f;
    style->TabBorderSize = 0.0f;
    style->WindowPadding = ImVec2(8, 8);
    style->WindowRounding = 0.0f;
    style->FramePadding = ImVec2(20, 1);
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

    // variable for sample count
    int sample_count = 1;
    // variable for the height of the sample per frame
    int y = 0;

    int mat_id = 0;

    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
    double render_1spp_time = 0.0;
    double render_avg = 0.0;

    int previous_y = 0; 
    bool render = false;
    bool user_stopped_render = false;
    bool save_window = false;

    // initializing gl texture for the renderview
    Render_View_Utils render_view_utils(settings.xres, settings.yres);

    //render_view_utils.buffer1 = (color_t*)malloc(xres * yres * sizeof(color_t));
    //render_view_utils.buffer2 = (color_t*)malloc(xres * yres * sizeof(color_t));

    // declaring all the editors
    Console console;
    Outliner outliner;
    MenuBar menubar;
    Geometry_Shelf geo_shelf;
    Light_Shelf light_shelf;
    Camera_Shelf cam_shelf;
    Render_View render_view;
    Render_View_Buttons rview_buttons;
    Save_Window rview_save_window;
    Editor editor;
    Render_Settings_Window rsettings_windows;

    // initializing the file dialog texture methods
    file_dialog_init();

    // declaring the edited variable to control the state of the renderer
    bool edited = false;
    int first_edit = 0;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // if parameters have been edited, rebuild the scene and reset the frame buffer and the sample counter
        if (render && edited)
        {
            if (first_edit == 1)
            {
                build_scene(settings.device, settings.scene, objects, materials, lights);
                first_edit = 2;
            }
            else if(first_edit > 1) rebuild_scene(settings.device, settings.scene, objects, materials, lights);
            reset_render(render_view_utils.buffer1, render_view_utils.buffer2, settings.xres, settings.yres, sample_count, y);
            edited = false;
        }

        // if user has pressed the stop button, wait until the progressive render has finished rendering the entire frame to stop the process
        if (render && user_stopped_render)
        {
            if (y == 0)
            {
                render = false;
                user_stopped_render = false;
            }
        }

        // demo window for ImGui
        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        // progressive render variables update
        previous_y = y;

        
        // Progressive rendering happens here
        if (render && sample_count > 1 && settings.integrator < 2)
        {
            progressive_render(sample_count, pixel_ids, y, sequence, render_view_utils.buffer1, settings, cameras[0], materials, lights, samples, bounces, render_stats);

#pragma omp parallel for
            for (int z = y; z < settings.yres; z++)
            {
                for (int x = 0; x < settings.xres; x++)
                {
                    if (z <= y + 50)
                    {
                        render_view_utils.buffer2[x + z * settings.xres].R = render_view_utils.buffer1[x + z * settings.xres].R / (float)sample_count;
                        render_view_utils.buffer2[x + z * settings.xres].G = render_view_utils.buffer1[x + z * settings.xres].G / (float)sample_count;
                        render_view_utils.buffer2[x + z * settings.xres].B = render_view_utils.buffer1[x + z * settings.xres].B / (float)sample_count;
                    }
                }
            }

            if (y < settings.yres)
            {
                try
                {
                    // apply the ocio view transform
                    OCIO::DisplayTransformRcPtr transform = OCIO::DisplayTransform::Create();
                    transform->setInputColorSpaceName(OCIO::ROLE_SCENE_LINEAR);
                    transform->setDisplay(ocio_utils.current_display);
                    transform->setView(ocio_utils.current_view);
                    OCIO::ConstProcessorRcPtr processor = ocio_config->getProcessor(transform);

                    int size = 51;
                    if (y == (settings.yres - 50)) size = 50;

                    OCIO::PackedImageDesc img(&render_view_utils.buffer2[y * settings.xres].R, settings.xres, size, 3);
                    processor->apply(img);
                }
                catch (OCIO::Exception& exception)
                {
                    std::cerr << "OCIO Error : " << exception.what() << "\n";
                }
            }

            
            glBindTexture(GL_TEXTURE_2D, render_view_utils.render_view_texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, settings.xres, settings.yres, GL_RGB, GL_FLOAT, render_view_utils.buffer2);
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
                sample_count++;

                //std::cout << y << "\n";

                y = 0;


                end = get_time();
                std::chrono::duration<double> elapsed = end - start;
                render_avg += elapsed.count();
            }

            if (y == previous_y) y = settings.yres;
        }
        
        

        if (render && sample_count < 2 || render && settings.integrator > 1)
        //if (render)
        {
            progressive_render_fast(sample_count, pixel_ids, sequence, render_view_utils.buffer1, settings, cameras[0], materials, lights, samples, bounces, render_stats);

#pragma omp parallel for
            for (int z = 0; z < settings.yres; z++)
            {
                for (int x = 0; x < settings.xres; x++)
                {
                    render_view_utils.buffer2[x + z * settings.xres].R = render_view_utils.buffer1[x + z * settings.xres].R / (float)sample_count;
                    render_view_utils.buffer2[x + z * settings.xres].G = render_view_utils.buffer1[x + z * settings.xres].G / (float)sample_count;
                    render_view_utils.buffer2[x + z * settings.xres].B = render_view_utils.buffer1[x + z * settings.xres].B / (float)sample_count;
                }
            }

            // apply the ocio view transform
            try
            {
                OCIO::DisplayTransformRcPtr transform = OCIO::DisplayTransform::Create();
                transform->setInputColorSpaceName(OCIO::ROLE_SCENE_LINEAR);
                transform->setDisplay(ocio_utils.current_display);
                transform->setView(ocio_utils.current_view);
                OCIO::ConstProcessorRcPtr processor = ocio_config->getProcessor(transform);

                OCIO::PackedImageDesc img(&render_view_utils.buffer2[0].R, settings.xres, settings.yres, 3);
                processor->apply(img);
            }
            catch (OCIO::Exception& exception)
            {
                std::cerr << "OCIO Error : " << exception.what() << "\n";
            }

            glBindTexture(GL_TEXTURE_2D, render_view_utils.render_view_texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, settings.xres, settings.yres, GL_RGB, GL_FLOAT, render_view_utils.buffer2);
            glBindTexture(GL_TEXTURE_2D, 0);

            sample_count++;
        }

        // draw the different windows

        // main menu bar
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));
        menubar.draw();
        ImGui::PopStyleVar();

        // Render Settings Editor
        rsettings_windows.draw(settings, render_view_utils, cameras, sequence, pixel_ids, sample_count, y, edited);

        // geometry shelf
        geo_shelf.draw(settings, objects, lights, cameras, console, edited, first_edit);

        // light shelf
        light_shelf.draw(settings, objects, lights, cameras, console, edited, first_edit);

        // cameras shelf
        cam_shelf.draw(settings, objects, lights, cameras, console, edited, first_edit);


        // render view
        rview_buttons.draw(render, user_stopped_render, render_view_utils, ocio_utils, sample_count, y, save_window);
        render_view.draw(render, render_view_utils, sample_count, y);

        // save window
        rview_save_window.draw(settings.xres, settings.yres, sample_count, render_view_utils.buffer1, save_window);

        // info window
        {
            ImGui::Begin("Infos");

            render_1spp_time = render_avg / sample_count;

            ImGui::Text("Rendered %i SPP", sample_count);
            
            ImGui::End();
        }

        // console
        bool o = true;
        console.Draw("Console", &o);

        // outliner
        outliner.draw(objects, cameras, lights, console, edited);

        // editor
        editor.draw(outliner, objects, lights, cameras, edited);


        // Rendering
        ImGui::Render();

        
        int display_w, display_h;

        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    // cleanup utility and buffers
    free(pixel_ids);
    free(render_view_utils.buffer1);
    free(render_view_utils.buffer2);

    return 0;
}
