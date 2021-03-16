#include "outliner.h"


void Outliner::draw(std::vector<Object>& objects, std::vector<Camera>& cameras, std::vector<Light*>& lights, Console& console)
{
	ImGui::Begin("Outliner");
	{

        int objects_count = 0;

        for (auto& object : objects)
        {
            if (ImGui::Selectable(object.name.c_str(), selected == objects_count))
            {
                selected = objects_count;
                selection_type = Selection_Type::SelectionType_Object;
            }

            objects_count++;
        }


        int camera_count = 0;

        for (auto& camera : cameras)
        {
            if (ImGui::Selectable(camera.name.c_str(), selected == camera_count))
            {
                selected = camera_count;
                selection_type = Selection_Type::SelectionType_Camera;
            }

            camera_count++;
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
                if (ImGui::Selectable(ptlight->name.c_str(), selected == light_count))
                {
                    selected = light_count;
                    selection_type = Selection_Type::SelectionType_Light;
                }
            }

            if (domelight = dynamic_cast<Dome_Light*>(light))
            {
                if (ImGui::Selectable(domelight->name.c_str(), selected == light_count))
                {
                    selected = light_count;
                    selection_type = Selection_Type::SelectionType_Light;
                }
            }

            if (dist_light = dynamic_cast<Distant_Light*>(light))
            {
                if (ImGui::Selectable(dist_light->name.c_str(), selected == light_count))
                {
                    selected = light_count;
                    selection_type = Selection_Type::SelectionType_Light;
                }
            }

            if (sq_light = dynamic_cast<Square_Light*>(light))
            {
                if (ImGui::Selectable(sq_light->name.c_str(), selected == light_count))
                {
                    selected = light_count;
                    selection_type = Selection_Type::SelectionType_Light;
                }
            }

            light_count++;
        }
	}

	ImGui::End();
}