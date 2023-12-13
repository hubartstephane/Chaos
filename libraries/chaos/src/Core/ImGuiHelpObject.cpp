#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiHelpObject::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		bool open_value = true;

		if (ImGui::Begin("Help", &open_value, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("F1:  Skip level");
			ImGui::Text("F2:  Toggle cheat mode");
			ImGui::Text("F3:  Reload game configuration");
			ImGui::Text("F4:  Reload current level");
			ImGui::Text("F5:  Quick save");
			ImGui::Text("F6:  Toggle free camera mode");
			ImGui::Text("F7:  Toggle imgui mode");
			ImGui::Text("F8:  Reload GPU resources");
			ImGui::Text("F9:  Screen captures");
			ImGui::Text("F10: Toggle fullscreen");

			ImGui::End();
		}

		if (!open_value)
			RequestClosing();
	}

}; // namespace chaos