#include "rendersettings.h"


void Render_Settings_Window::draw(Render_Settings& settings, Render_View_Utils& utils, 
                                  std::vector<Camera>& cameras, std::vector<std::vector<vec2>>& sequence,
                                  int*& pixel_ids, int& sample_count, int& y, bool& change)
{
    ImGui::Begin("Render Settings");

    static bool reset = false;

    if (ImGui::CollapsingHeader("General"))
    {
        ImGui::Text("Resolution");
        ImGui::SameLine();
        ImGui::PushItemWidth(60.0f);
        ImGui::DragInt("X", &settings.xres);
        ImGui::SameLine();
        ImGui::DragInt("Y", &settings.yres);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        if(ImGui::SmallButton("Submit")) reset = true;


        if (reset)
        {
            utils.resolution.x = settings.xres;
            utils.resolution.y = settings.yres;

            utils.scrolling = ImVec2(0, 0);

            cameras[0].aspect = (float)settings.xres / (float)settings.yres;

            free(utils.buffer1); utils.buffer1 = nullptr;
            free(utils.buffer2); utils.buffer2 = nullptr;
            free(pixel_ids); pixel_ids = nullptr;

            pixel_ids = (int*)malloc(settings.xres * settings.yres * sizeof(int));

#pragma omp parallel for
            for (int i = 0; i < settings.xres * settings.yres; i++)
            {
                pixel_ids[i] = (int)(generate_random_float_fast(i) * (sequence.size() - 2));
            }
            
            utils.buffer1 = (color_t*)malloc(settings.xres * settings.yres * sizeof(color_t));
            utils.buffer2 = (color_t*)malloc(settings.xres * settings.yres * sizeof(color_t));

            glBindTexture(GL_TEXTURE_2D, utils.render_view_texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, settings.xres, settings.yres, 0, GL_RGB, GL_FLOAT, utils.buffer2);
            glBindTexture(GL_TEXTURE_2D, 0);

            reset_render(utils.buffer1, utils.buffer2, settings.xres, settings.yres, sample_count, y);
            
            change = false;
            reset = false;
        }
    }

    if (ImGui::CollapsingHeader("Integrator"))
    {
        const char* items[] = { "Pathtracer", "Ambient Occlusion", "Scene Viewer" };

        ImGui::Combo("Mode", &settings.integrator, items, IM_ARRAYSIZE(items));
        if (ImGui::IsItemEdited()) { change = true; }
    }


    ImGui::End();
}
