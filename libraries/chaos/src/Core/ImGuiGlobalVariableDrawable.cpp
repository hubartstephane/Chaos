#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiGlobalVariableDrawable::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		bool open_value = true;

		ImGui::Begin("Help", &open_value, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);


		ImGui::End();

		if (!open_value)
			RequestClosing();
	}

}; // namespace chaos