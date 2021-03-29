#include "outliner.h"


void Outliner::draw(std::vector<Object>& objects, std::vector<Camera>& cameras, std::vector<Light*>& lights, Console& console, bool& edited)
{
	ImGui::Begin("Outliner");
	{
        int objects_count = 0;
        int total_count = 0;

        for (auto& object : objects)
        {
            if (ImGui::Selectable(object.name.c_str(), selected == total_count))
            {
                selected = total_count;
                type_selected = objects_count;
                selection_type = Selection_Type::SelectionType_Object;
            }

            objects_count++;
            total_count++;
        }


        int camera_count = 0;

        for (auto& camera : cameras)
        {
            if (ImGui::Selectable(camera.name.c_str(), selected == total_count))
            {
                selected = total_count;
                type_selected = camera_count;
                selection_type = Selection_Type::SelectionType_Camera;
            }

            camera_count++;
            total_count++;
        }


        int light_count = 0;

        for (auto& light : lights)
        {
            // dynamic casting light pointers to get the light type
            Point_Light* ptlight = nullptr;
            Dome_Light* domelight = nullptr;
            Distant_Light* dist_light = nullptr;
            Square_Light* sq_light = nullptr;

            if (ptlight = dynamic_cast<Point_Light*>(light))
            {
                if (ImGui::Selectable(ptlight->name.c_str(), selected == total_count))
                {
                    selected = total_count;
                    type_selected = light_count;
                    selection_type = Selection_Type::SelectionType_Light;
                }
            }

            if (domelight = dynamic_cast<Dome_Light*>(light))
            {
                if (ImGui::Selectable(domelight->name.c_str(), selected == total_count))
                {
                    selected = total_count;
                    type_selected = light_count;
                    selection_type = Selection_Type::SelectionType_Light;
                }
            }

            if (dist_light = dynamic_cast<Distant_Light*>(light))
            {
                if (ImGui::Selectable(dist_light->name.c_str(), selected == total_count))
                {
                    selected = total_count;
                    type_selected = light_count;
                    selection_type = Selection_Type::SelectionType_Light;
                }
            }

            if (sq_light = dynamic_cast<Square_Light*>(light))
            {
                if (ImGui::Selectable(sq_light->name.c_str(), selected == total_count))
                {
                    selected = total_count;
                    type_selected = light_count;
                    selection_type = Selection_Type::SelectionType_Light;
                }
            }

            light_count++;
            total_count++;
        }

        // deleting the selected item with the delete key
        if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)) ||
            ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
        {
            edited = true;

            if (selection_type == Selection_Type::SelectionType_Object)
            {   
                // shortcut : s = type_selected variable
                int s = type_selected;

                // release the object (free memory and embree geometry)
                objects[s].release();

                objects.erase(objects.begin() + s);
            }

            if (selection_type == Selection_Type::SelectionType_Camera)
            {
                int s = type_selected;

                cameras.erase(cameras.begin() + s);
            }

            if (selection_type == Selection_Type::SelectionType_Light)
            {
                int s = type_selected;

                // free the memory to avoid mem leaks
                delete lights[s];

                lights.erase(lights.begin() + s);
            }
        }

        // duplicate selected item in the outliner
        if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
        {
            edited = true;

            if (selection_type == Selection_Type::SelectionType_Object)
            {
                // shortcut : s = type_selected variable
                int s = type_selected;
                Object duplicate(objects[s]);
                duplicate.name = increment_name(duplicate.name);

                objects.push_back(duplicate);
            }

            if (selection_type == Selection_Type::SelectionType_Camera)
            {
                int s = type_selected;
                Camera duplicate(cameras[s]);
                duplicate.name = increment_name(duplicate.name);

                cameras.push_back(duplicate);
            }

            if (selection_type == Selection_Type::SelectionType_Light)
            {
                int s = type_selected;
                Light* duplicate(lights[s]);
                duplicate->name = increment_name(duplicate->name);

                lights.push_back(duplicate);
            }
        }
	}

	ImGui::End();
}