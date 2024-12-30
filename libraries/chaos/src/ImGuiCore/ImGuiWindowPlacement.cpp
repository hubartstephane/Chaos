#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiWindowPlacement ImGuiWindowPlacement::GetCenterOnScreenPlacement(const glm::vec2& in_alignment)
	{
		ImGuiWindowPlacement result;
		result.placement_type = ImGuiWindowPlacementType::ScreenCenter;
		result.alignment = in_alignment;
		return result;
	}

	ImGuiWindowPlacement ImGuiWindowPlacement::GetCenterOnCursorPlacement(const glm::vec2& in_alignment)
	{
		ImGuiWindowPlacement result;
		result.placement_type = ImGuiWindowPlacementType::CursorPosition;
		result.alignment = in_alignment;
		return result;
	}

	ImGuiWindowPlacement ImGuiWindowPlacement::GetCenterOnPositionPlacement(const glm::vec2& in_position, const glm::vec2& in_alignment)
	{
		ImGuiWindowPlacement result;
		result.placement_type = ImGuiWindowPlacementType::FreePosition;
		result.alignment = in_alignment;
		result.position = in_position;
		return result;
	}

	void ImGuiWindowPlacement::PrepareNextWindowPlacement() const
	{
		ImVec2 imgui_position = { 0.0f, 0.0f };
		ImVec2 imgui_alignment = { alignment.x, alignment.y };

		if (placement_type == ImGuiWindowPlacementType::ScreenCenter)
			imgui_position = ImGui::GetMainViewport()->GetCenter();
		else if (placement_type == ImGuiWindowPlacementType::CursorPosition)
			imgui_position = ImGui::GetMousePos();
		else if (placement_type == ImGuiWindowPlacementType::FreePosition)
			imgui_position = { position.x, position.y };

		ImGui::SetNextWindowPos(imgui_position, ImGuiCond_Always, imgui_alignment);
	}

}; // namespace chaos