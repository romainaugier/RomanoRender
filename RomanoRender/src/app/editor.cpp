#include "editor.h"


void Editor::draw(Outliner& outliner, std::vector<Object>& objects, std::vector<Light*>& lights, std::vector<Camera>& cameras, bool& edited)
{
	if (objects.size() > 0 || cameras.size() > 0 || lights.size() > 0)
	{
		ImGui::Begin("Editor");
		// Object / Material Editor
		if (outliner.selection_type == Selection_Type::SelectionType_Object)
		{
			// shortcut
			int s = outliner.type_selected;

			if (ImGui::CollapsingHeader("Properties"))
			{

				ImGui::Text("Geometry : %s", objects[s].name.c_str());

				if (ImGui::CollapsingHeader("Transform"))
				{
					ImGui::DragFloat3("Object Translate", &objects[s].translate.x, 0.1f);
					if (ImGui::IsItemEdited()) { objects[s].set_transform(); edited = true; }

					ImGui::DragFloat3("Object Rotate", &objects[s].rotate.x, 0.1f);
					if (ImGui::IsItemEdited()) { objects[s].set_transform(); edited = true; }

					ImGui::DragFloat3("Object Scale", &objects[s].scale.x, 0.1f);
					if (ImGui::IsItemEdited()) { objects[s].set_transform(); edited = true; }
				}
			}
			
			if(ImGui::CollapsingHeader("Material"))
			{ 
				ImGui::Text("Material : %s", objects[s].material.name.c_str());

				if (ImGui::CollapsingHeader("Diffuse"))
				{
					ImGui::ColorEdit3("Diffuse Color", &objects[s].material.diffuse_color.x, ImGuiColorEditFlags_Float);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragFloat("Diffuse Weight", &objects[s].material.diffuse_weight, 0.05f, 0.0f, 1.0f);
					if (ImGui::IsItemEdited()) { edited = true; }
				}

				if (ImGui::CollapsingHeader("Reflection"))
				{
					ImGui::ColorEdit3("Reflection Color", &objects[s].material.reflectance_color.x, ImGuiColorEditFlags_Float);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragFloat("Reflection Weight", &objects[s].material.reflectance, 0.05f, 0.0f, 1.0f);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragFloat("Reflection Roughness", &objects[s].material.reflectance_roughness, 0.05f, 0.0f, 1.0f);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragFloat("Metallic", &objects[s].material.metallic, 0.05f, 0.0f, 1.0f);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragFloat3("Ior", &objects[s].material.ior.x, 0.05, 0.0f, 5.0f);
					if (ImGui::IsItemEdited()) { edited = true; }
				}

				if (ImGui::CollapsingHeader("Refraction/Transmission"))
				{
					ImGui::ColorEdit3("Refraction Color", &objects[s].material.refraction_color.x, ImGuiColorEditFlags_Float);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragFloat("Refraction Weight", &objects[s].material.refraction, 0.05f, 0.0f, 1.0f);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragFloat("Refraction Roughness", &objects[s].material.refraction_roughness, 0.05f, 0.0f, 1.0f);
					if (ImGui::IsItemEdited()) { edited = true; }
				}

				if (ImGui::CollapsingHeader("Subsurface Scattering"))
				{
					ImGui::ColorEdit3("Surface Color", &objects[s].material.sss_color.x, ImGuiColorEditFlags_Float);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragFloat("Subsurface Weigth", &objects[s].material.sss, 0.05f, 0.0f, 1.0f);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragFloat("Scale", &objects[s].material.sss_scale, 0.05f, 0.0f, 10.0f);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::ColorEdit3("Radius Color", &objects[s].material.sss_radius.x, ImGuiColorEditFlags_Float);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragFloat("Attenuation", &objects[s].material.sss_abs, 0.05f, 0.0f, 1.0f);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragInt("Steps", &objects[s].material.sss_steps, 1, 20);
					if (ImGui::IsItemEdited()) { edited = true; }
				}
			}
		}

		// Camera Editor
		else if (outliner.selection_type == Selection_Type::SelectionType_Camera)
		{
			// shortcut
			int s = outliner.type_selected;

			if (ImGui::CollapsingHeader("Properties") && cameras.size() > 0)
			{
				ImGui::Text("Camera : %s", cameras[s].name.c_str());

				ImGui::DragFloat3("Position", &cameras[s].pos.x, 0.1f);
				if (ImGui::IsItemEdited()) { cameras[s].set_transform();  edited = true; }

				ImGui::DragFloat3("Rotation", &cameras[s].rotation.x, 0.1f);
				if (ImGui::IsItemEdited()) { cameras[s].set_transform(); edited = true; }

				ImGui::DragFloat("Focal Length", &cameras[s].focal_length, 1, 200);
				if (ImGui::IsItemEdited()) { edited = true; }

				ImGui::DragFloat("Bokeh Power", &cameras[s].aperture, 0.0f, 2.0f);
				if (ImGui::IsItemEdited()) { edited = true; }

				ImGui::DragFloat("Focus Distance", &cameras[s].focus_dist, 0.0f, 200.0f);
				if (ImGui::IsItemEdited()) { edited = true; }
			}
		}

		// Light Editor
		else if (outliner.selection_type == Selection_Type::SelectionType_Light)
		{
			// shortcut
			int s = outliner.type_selected;

			// null ptrs to dynamic cast to get each light properties
			Point_Light* ptlight = nullptr;
			Distant_Light* distlight = nullptr;
			Square_Light* sqlight = nullptr;
			Dome_Light* domelight = nullptr;

			if (lights.size() > 0) ImGui::Text("Light : %s", lights[s]->name.c_str());

			if (ImGui::CollapsingHeader("General") && lights.size() > 0)
			{
				ImGui::ColorEdit3("Light Color", &lights[s]->color.x, ImGuiColorEditFlags_Float);
				if (ImGui::IsItemEdited()) { edited = true; }

				if (sqlight = dynamic_cast<Square_Light*>(lights[s]))
				{
					ImGui::DragFloat("Light Intensity", &lights[s]->intensity, 0.5f, 0.0f, 5000.0f);
					if (ImGui::IsItemEdited()) { edited = true; }
				}
				else
				{
					ImGui::DragFloat("Light Intensity", &lights[s]->intensity, 0.05f, 0.0f, 10.0f);
					if (ImGui::IsItemEdited()) { edited = true; }
				}
			}

			if (ImGui::CollapsingHeader("Properties") && lights.size() > 0)
			{
				if (ptlight = dynamic_cast<Point_Light*>(lights[s]))
				{
					ImGui::DragFloat3("Light Position", &ptlight->position.x, 0.05f, -100.0f, 100.0f);
					if (ImGui::IsItemEdited()) { edited = true; }
				}

				if (distlight = dynamic_cast<Distant_Light*>(lights[s]))
				{
					ImGui::DragFloat3("Light Orientation", &distlight->orientation.x, 0.02f, -1.0f, 1.0f);
					if (ImGui::IsItemEdited()) { edited = true; }

					ImGui::DragFloat("Light Angle", &distlight->angle, 0.02f, 0.01f, 10.0f);
					if (ImGui::IsItemEdited()) { edited = true; }
				}

				if (sqlight = dynamic_cast<Square_Light*>(lights[s]))
				{
					ImGui::DragFloat3("Light Translate", &sqlight->translate.x, 0.05f, -100.0f, 100.0f);
					if (ImGui::IsItemEdited()) { sqlight->set_transform();  edited = true; }

					ImGui::DragFloat3("Light Rotate", &sqlight->rotate.x, 0.05f, -360.0f, 360.0f);
					if (ImGui::IsItemEdited()) { sqlight->set_transform(); edited = true; }

					ImGui::DragFloat2("Light Size", &sqlight->size.x, 0.05f, 0.1f, 100.0f);
					if (ImGui::IsItemEdited()) { sqlight->set_transform(); edited = true; }
				}

				if (domelight = dynamic_cast<Dome_Light*>(lights[s]))
				{
					ImGui::Text("Dome Light");
				}
			}
		}

		// Void Editor
		else
		{

		}
		ImGui::End();
	}
}