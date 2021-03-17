#include "editor.h"


void Editor::draw(Outliner& outliner, std::vector<Object>& objects, std::vector<Light*> lights, std::vector<Camera>& cameras, bool& edited)
{
	ImGui::Begin("Editor");
	// Object / Material Editor
	if (outliner.selection_type == Selection_Type::SelectionType_Object)
	{
		ImGui::BeginChild("Geometry");
		// shortcut
		int s = outliner.type_selected;

		ImGui::Text("Geometry : %s", objects[s].name.c_str());

		ImGui::DragFloat3("Translate", &objects[s].translate.x, 0.1f);
		if (ImGui::IsItemEdited()) { objects[s].set_transform(); edited = true; }

		ImGui::DragFloat3("Rotate", &objects[s].rotate.x, 0.1f);
		if (ImGui::IsItemEdited()) { objects[s].set_transform(); edited = true; }

		ImGui::DragFloat3("Scale", &objects[s].scale.x, 0.1f);
		if (ImGui::IsItemEdited()) { objects[s].set_transform(); edited = true; }


		ImGui::EndChild();
	}

	// Camera Editor
	else if (outliner.selection_type == Selection_Type::SelectionType_Camera)
	{
		ImGui::BeginChild("Camera");

		// shortcut
		int s = outliner.type_selected;

		ImGui::Text("Camera : %s", cameras[s].name.c_str());

		ImGui::DragFloat3("Position", &cameras[s].pos.x, 0.1f);
		if (ImGui::IsItemEdited()) { edited = true; }

		ImGui::DragFloat3("Aim", &cameras[s].lookat.x, 0.1f);
		if (ImGui::IsItemEdited()) { edited = true; }

		ImGui::DragFloat("Focal Length", &cameras[s].focal_length, 1, 200);
		if (ImGui::IsItemEdited()) { edited = true; }

		ImGui::DragFloat("Bokeh Power", &cameras[s].aperture, 0.0f, 2.0f);
		if (ImGui::IsItemEdited()) { edited = true; }

		ImGui::DragFloat("Focus Distance", &cameras[s].focus_dist, 0.0f, 200.0f);
		if (ImGui::IsItemEdited()) { edited = true; }

		ImGui::EndChild();
	}

	// Light Editor
	else if (outliner.selection_type == Selection_Type::SelectionType_Light)
	{
		ImGui::BeginChild("Light");
		// shortcut
		int s = outliner.type_selected;

		ImGui::Text("Light : %s", lights[s]->name.c_str());

		ImGui::EndChild();
	}

	// Void Editor
	else
	{

	}
	ImGui::End();
}