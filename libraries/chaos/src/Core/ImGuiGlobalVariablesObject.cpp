#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiGlobalVariablesObject::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		bool open_value = true;

		ImGui::Begin("Help", &open_value, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

		for (GlobalVariableBase * variable : chaos::GlobalVariableManager::GetInstance()->GetVariables())
		{
			if (variable != nullptr)
			{
				if (GlobalVariableInfo const* variable_info = variable->GetVariableInfo())
				{
					if (GlobalVariableImGUIRendererBase const* imgui_renderer = variable_info->GetImGUIRenderer())
					{
						imgui_renderer->DrawImGui(variable);
					}
				}
			}
		}

		ImGui::End();

		if (!open_value)
			RequestClosing();
	}

}; // namespace chaos