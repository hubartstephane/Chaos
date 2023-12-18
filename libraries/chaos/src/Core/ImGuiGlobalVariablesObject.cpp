#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiGlobalVariablesObject::DrawImGui(ImGuiDrawMenuMode menu_mode)
	{
		bool open_value = true;

		int id = 0;

		if (ImGui::Begin("Global Variables", &open_value, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (ImGui::BeginTable("Global Variables", 2))
			{
				for (GlobalVariableBase* variable : chaos::GlobalVariableManager::GetInstance()->GetVariables())
				{
					if (variable != nullptr)
					{
						if (GlobalVariableInfo const* variable_info = variable->GetVariableInfo())
						{
							if (GlobalVariableImGuiRendererBase const* imgui_renderer = variable_info->GetImGuiRenderer())
							{
								//ImGui::PushID(++id);
								ImGui::TableNextColumn();
								ImGui::Text("%s", variable->GetName());
								//ImGui::PopID();

								//ImGui::PushID(++id);
								ImGui::TableNextColumn();
								imgui_renderer->DrawImGui(variable);
								//ImGui::PopID();

							}
						}
					}
				}
				ImGui::EndTable();
			}
			ImGui::End();
		}

		if (!open_value)
			RequestClosing();
	}

}; // namespace chaos