#include "rendersettings.h"


void Render_Settings_Window::draw(Render_Settings& settings, Render_View_Utils& utils, 
                                  std::vector<Camera>& cameras, std::vector<std::vector<vec2>>& sequence, 
                                  color_t* pixels, color_t* new_pixels, 
                                  int* pixel_ids, int& sample_count, int& y, bool& change)
{
    ImGui::Begin("Render Settings");

    static bool reset = false;

    if (ImGui::CollapsingHeader("General"))
    {
        ImGui::Text("Resolution");
        ImGui::SameLine();
        ImGui::PushItemWidth(60.0f);
        ImGui::DragInt("X", &settings.xres);
        if (ImGui::IsItemEdited()) { change = true; reset = true; }
        ImGui::SameLine();
        ImGui::DragInt("Y", &settings.yres);
        if (ImGui::IsItemEdited()) { change = true; reset = true; }
        ImGui::PopItemWidth();


        if (reset)
        {
            utils.resolution.x = settings.xres;
            utils.resolution.y = settings.yres;

            utils.scrolling = ImVec2(0, 0);

            cameras[0].aspect = (float)settings.xres / (float)settings.yres;

            free(pixels);
            free(new_pixels);
            delete pixel_ids;

            pixel_ids = new int[settings.xres * settings.yres];

#pragma omp parallel for
            for (int i = 0; i < settings.xres * settings.yres - 1; i++)
            {
                pixel_ids[i] = (int)(generate_random_float_fast(i) * (sequence.size() - 1));
            }

            pixels = (color_t*)malloc(settings.xres * settings.yres * sizeof(color_t));
            new_pixels = (color_t*)malloc(settings.xres * settings.yres * sizeof(color_t));

            glBindTexture(GL_TEXTURE_2D, utils.render_view_texture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, settings.xres, settings.yres, 0, GL_RGB, GL_FLOAT, new_pixels);

            glBindTexture(GL_TEXTURE_2D, 0);

            reset_render(pixels, new_pixels, settings.xres, settings.yres, sample_count, y);

            change = false;
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
