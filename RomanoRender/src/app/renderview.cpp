#include "renderview.h"


namespace OCIO = OCIO_NAMESPACE;


void Render_View::draw(bool& render, Render_View_Utils& utils, int& s, int& y)
{
    {
        ImGui::Begin("RenderView");

        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImVec2 size(utils.resolution.x, utils.resolution.y);

        ImVec2 origin(0.5f, 0.5f);

        const ImVec4 tint(1.0f, 1.0f, 1.0f, 1.0f);
        const ImVec4 border_color(0.0f, 0.0f, 0.0f, 1.0f);

        float z = (utils.zoom < 1.0f) ? utils.zoom : 1.0f;
        z = (utils.zoom > 0.1f) ? utils.zoom : 0.1;


        const bool is_hovered = ImGui::IsWindowHovered();
        const bool is_active = ImGui::IsWindowFocused();

        if (is_hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
        {
            utils.scrolling.x += io.MouseDelta.x * 1.0f;
            utils.scrolling.y += io.MouseDelta.y * 1.0f;
        }

        const float mouse_wheel = io.MouseWheel;

        if (is_hovered && mouse_wheel != 0.0f)
        {
            utils.zoom -= (mouse_wheel * 0.1f);
        }

        ImGui::PushItemFlag(ImGuiWindowFlags_NoScrollWithMouse, 1);

        ImGui::SetScrollY(0.0f);

        ImGui::SetCursorPos((ImGui::GetWindowSize() - size * z) * 0.5f + utils.scrolling);
        ImGui::Image((void*)utils.render_view_texture, size * z, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), tint, border_color);

        ImGui::PopItemFlag();

        ImGui::End();
    }
}


void Render_View_Buttons::draw(bool& render, Render_View_Utils& utils, OCIO_Utils& ocio_utils, int& s, int& y, bool& save_window)
{
    ImGui::Begin("Render Commands");
    {
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
            reset_render(utils.buffer1, utils.buffer2, utils.resolution.x, utils.resolution.y, s, y);
            utils.scrolling = ImVec2(0.0f, 0.0f);
            utils.zoom = 1.0f;
        }

        ImGui::SameLine();

        if (ImGui::Button("Save Image"))
        {
            save_window = true;
        }

        ImGui::SameLine();
        
        ImVec2 win_width = ImGui::GetContentRegionAvail();
        ImGui::Dummy(ImVec2(win_width.x - (150.0f + ImGui::GetFontSize() * 5), 0.0f));

        ImGui::SameLine();

        static int current_item = 0;

        ImGui::PushItemWidth(150.0f);
        ImGui::Combo("Views", &current_item, &ocio_utils.active_views[0], ocio_utils.active_views.size());
        ImGui::PopItemWidth();

        ocio_utils.current_view = ocio_utils.active_views[current_item];
    }
    ImGui::End();
}


void Save_Window::draw(int xres, int yres, int sample_count, color_t* image_buffer, bool& open)
{
    if (open)
    {
        ImGui::Begin("Save Image", &open);

        static char path[512] = "D:/image.exr";

        ImGui::InputText("File Path", path, IM_ARRAYSIZE(path));

        if (ImGui::Button("Save"))
        {
            color_t* out_pixels = (color_t*)malloc(xres * yres * sizeof(color_t));

            for (int z = 0; z < yres; z++)
            {
                for (int x = 0; x < xres; x++)
                {
                    out_pixels[x + z * xres].R = image_buffer[x + z * xres].R / sample_count;
                    out_pixels[x + z * xres].G = image_buffer[x + z * xres].G / sample_count;
                    out_pixels[x + z * xres].B = image_buffer[x + z * xres].B / sample_count;
                }
            }

            OIIO::ImageSpec spec(xres, yres, 3, OIIO::TypeDesc::FLOAT);
            OIIO::ImageBuf buffer(spec, out_pixels);

            buffer.write(path);

            delete[] out_pixels;

            open = false;
        }

        ImGui::End();
    }
    else return;
}