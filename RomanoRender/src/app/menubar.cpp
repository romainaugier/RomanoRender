#include "menubar.h"


void MenuBar::draw()
{
	if(ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save")) {}
			if (ImGui::MenuItem("Save As")) {}
			if (ImGui::MenuItem("New")) {}
			if (ImGui::MenuItem("Open")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo")) {}
			if (ImGui::MenuItem("Redo")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Layout"))
		{
			if (ImGui::MenuItem("Reset")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Render"))
		{
			if (ImGui::MenuItem("Render")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation")) {}

			ImGui::EndMenu();
		}

		ImVec2 win_width = ImGui::GetContentRegionAvail();
		ImGui::Dummy(ImVec2(win_width.x - 150.0f, 0.0f));

		// widgets for cpu and ram usage
#undef max
		float cpu_usage = std::max(0.1f, GetCPULoad());
		ImVec4 cpu_color = ImVec4(cpu_usage, 1.0f - cpu_usage, 0.0f, 1.0f);

		ImGui::PushItemWidth(20.0f);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, cpu_color);
		ImGui::PlotHistogram("CPU", &cpu_usage, 1, 0, NULL, 0.0f, 1.0f, ImVec2(0, 25.0f));
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();

		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);
		GlobalMemoryStatusEx(&statex);

		float memory_usage = statex.dwMemoryLoad / 100.0f;
		ImVec4 mem_color = ImVec4(memory_usage, 1.0f - memory_usage, 0.0f, 1.0f);

		ImGui::PushItemWidth(20.0f);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, mem_color);
		ImGui::PlotHistogram("RAM", &memory_usage, 1, 0, NULL, 0.0f, 1.0f, ImVec2(0, 25.0f));
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();

		ImGui::EndMainMenuBar();
	}
}