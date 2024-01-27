#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiGlobalVariablesObject::OnDrawImGuiContent()
	{
		if (ImGui::BeginTable("Global Variables", 2, ImGuiTableFlags_Resizable))
		{
			for (GlobalVariableBase* variable : chaos::GlobalVariableManager::GetInstance()->GetVariables())
			{
				if (variable != nullptr)
				{
					if (GlobalVariableInfo const* variable_info = variable->GetVariableInfo())
					{
						if (GlobalVariableImGuiRendererBase const* imgui_renderer = variable_info->GetImGuiRenderer())
						{
							if (imgui_renderer->CanDrawVariable())
							{
								ImGui::TableNextColumn();
								ImGui::Text("%s", variable->GetName());

								ImGui::TableNextColumn();
								imgui_renderer->DrawVariable(variable);





								
								
							}
						}
					}
				}
			}
			ImGui::EndTable();


			ImGui::BeginDisabled();

			static glm::vec4 color = { 0.0f, 0.0f, 0.0f , 1.0f};
			DrawImGuiVariableImpl(color, DrawImGuiVariableFlags::COLOR);


			ImGui::EndDisabled();

			//static float col[4] = { 0.0f, 0.0f, 0.0f , 0.0f};
			//ImGui::ColorEdit4("colour", col, 0);

			//static ImVec4 col2 = { 0.0f, 0.0f, 0.0f, 0.0f };
			//ImGui::ColorButton("desc_id", col2);



			static std::vector<std::string> names = { "name1" , "name2", "name3", "name4", "name5", "name6" };


			int move_from = -1, move_to = -1;
			for (int i = 0; i < names.size() ; ++i)
			{
				

				ImGui::Button(names[i].c_str());
				

				ImGuiDragDropFlags src_flags = 0;
				src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
				src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
				//src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
				if (ImGui::BeginDragDropSource(src_flags))
				{
					ImGui::SetDragDropPayload("DND_DEMO_NAME", &i, sizeof(int));
					if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
					{
						ImGui::Text(names[i].c_str()); ImGui::SameLine(); ImGui::Button(names[i].c_str());
					}
					ImGui::EndDragDropSource();
				}

				






				if (ImGui::BeginDragDropTarget())
				{
					ImGuiDragDropFlags target_flags = 0;
					target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
					//target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_NAME", target_flags))
					{
						move_from = *(const int*)payload->Data;
						move_to = i;
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine(); ImGui::Text(names[i].c_str()); ImGui::SameLine(); ImGui::Button(names[i].c_str());
				







			}


			if (move_from != -1 && move_to != -1)
			{
				std::swap(names[move_from], names[move_to]);
				ImGui::SetDragDropPayload("DND_DEMO_NAME", &move_to, sizeof(int)); // Update payload immediately so on the next frame if we move the mouse to an earlier item our index payload will be correct. This is odd and showcase how the DnD api isn't best presented in this example.
	
			}


		}
	}

}; // namespace chaos