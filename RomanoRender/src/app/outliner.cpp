#include "outliner.h"


void Outliner::draw(std::vector<Camera>& cameras, std::vector<Light*>& lights, Console& console)
{
	ImGui::Begin("Outliner");
	{
        int i = 0;
        for (auto& camera : cameras)
        {
            if (ImGui::Selectable(camera.name.c_str(), selected == i))
            {
                selected = i;
            }

            i++;
        }

        for (auto& light : lights)
        {
            // dynamic casting light pointers to get the light type
            Point_Light* ptlight = nullptr;
            Dome_Light* domelight = nullptr;
            Distant_Light* dist_light = nullptr;
            Square_Light* sq_light = nullptr;

            if (ptlight = dynamic_cast<Point_Light*>(light))
            {
                if (ImGui::Selectable(ptlight->name.c_str(), selected == i))
                {
                    selected = i;
                }
            }

            if (domelight = dynamic_cast<Dome_Light*>(light))
            {
                if (ImGui::Selectable(domelight->name.c_str(), selected == i))
                {
                    selected = i;
                }
            }

            if (dist_light = dynamic_cast<Distant_Light*>(light))
            {
                if (ImGui::Selectable(dist_light->name.c_str(), selected == i))
                {
                    selected = i;
                }
            }

            if (sq_light = dynamic_cast<Square_Light*>(light))
            {
                if (ImGui::Selectable(sq_light->name.c_str(), selected == i))
                {
                    selected = i;
                }
            }

            i++;
        }
	}

	ImGui::End();
}