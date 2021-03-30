#include "shelf.h"


void Shelf::draw(Render_Settings& settings, std::vector<Object>& objects, std::vector<Light*>& lights, std::vector<Camera>& cameras, Console& console, bool& change, int& edit)
{

}


void Geometry_Shelf::draw(Render_Settings& settings, std::vector<Object>& objects, std::vector<Light*>& lights, std::vector<Camera>& cameras, Console& console, bool& change, int& edit)
{
	ImGui::Begin("Geometry");
	{
		if (ImGui::Button("OBJ"))
		{
			ifd::FileDialog::Instance().Open("ObjOpenDialog", "Select an obj file", "Obj file (*.obj;*.OBJ){.obj,.OBJ},.*", true);
		}

		if (ifd::FileDialog::Instance().IsDone("ObjOpenDialog"))
		{
			if (ifd::FileDialog::Instance().HasResult())
			{
				const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
				for (const auto& r : res)
				{
					load_object(settings.device, r.u8string(), objects, console);
				}

				change = true;
				if (edit < 1) edit = 1;
			}

			ifd::FileDialog::Instance().Close();
		}

		ImGui::SameLine();

		if (ImGui::Button("ABC"))
		{
			ifd::FileDialog::Instance().Open("AbcOpenDialog", "Select an alembic file", "Alembic file (*.abc){.abc},.*", true);
		}

		if (ifd::FileDialog::Instance().IsDone("AbcOpenDialog"))
		{
			if (ifd::FileDialog::Instance().HasResult())
			{
				const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
				for (const auto& r : res) printf("Open : %s\n", r.u8string().c_str());
			}
			ifd::FileDialog::Instance().Close();
		}
	}

	ImGui::End();
}


void Light_Shelf::draw(Render_Settings& settings, std::vector<Object>& objects, std::vector<Light*>& lights, std::vector<Camera>& cameras, Console& console, bool& change, int& edit)
{
	ImGui::Begin("Light");
	{
		if (ImGui::Button("Point"))
		{
			lights.push_back(new Point_Light());
			change = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Dome"))
		{
			lights.push_back(new Dome_Light());
			change = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Distant"))
		{
			lights.push_back(new Distant_Light());
			change = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Square"))
		{
			lights.push_back(new Square_Light());
			change = true;
		}
	}

	ImGui::End();
}


void Camera_Shelf::draw(Render_Settings& settings, std::vector<Object>& objects, std::vector<Light*>& lights, std::vector<Camera>& cameras, Console& console, bool& change, int& edit)
{
	ImGui::Begin("Camera");
	{
		if (ImGui::Button("Camera"))
		{
			cameras.push_back(Camera(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), 50, settings.xres, settings.yres, 0.0f, 20.0f, 1.0f, 1.0f));
			change = true;
		}
	}

	ImGui::End();
}