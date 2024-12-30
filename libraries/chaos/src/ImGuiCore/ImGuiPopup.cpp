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

	namespace details
	{
		ImGuiPopupState ImGuiPopupBase::GetPopupState() const
		{
			return popup_state;
		}

		void ImGuiPopupBase::Close()
		{
			if (popup_state == ImGuiPopupState::Closed)
				return;

			if (popup_state == ImGuiPopupState::Opened)
				ImGui::CloseCurrentPopup();

			popup_name = {};
			popup_state = ImGuiPopupState::Closed;
		}

		int ImGuiPopupBase::GetPopupFlags() const
		{
			return imgui_flags;
		}

		void ImGuiPopupBase::SetPopupFlags(int in_flags)
		{
			imgui_flags = in_flags;
		}

		bool ImGuiPopupBase::DoOpen(std::string in_popup_name, const ImGuiWindowPlacement& in_placement)
		{
			if (popup_state != ImGuiPopupState::Closed)
				return false;

			popup_name = std::move(in_popup_name);
			popup_placement = in_placement;
			popup_state = ImGuiPopupState::Opening;
			return true;
		}

		void ImGuiPopupBase::PrepareModalPlacement()
		{
			if (popup_state == ImGuiPopupState::Opening)
			{
				ImGui::OpenPopup(popup_name.c_str(), ImGuiPopupFlags_MouseButtonRight);
				popup_placement.PrepareNextWindowPlacement();
				popup_state = ImGuiPopupState::Opened;
			}
		}

	}; // namespace details

}; // namespace chaos