#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	PopupPlacement PopupPlacement::GetCenterOnScreenPlacement(bool in_movable, const glm::vec2& in_alignment)
	{
		PopupPlacement result;
		result.placement_type = PopupPlacementType::ScreenCenter;
		result.movable = in_movable;
		result.alignment = in_alignment;
		return result;
	}

	PopupPlacement PopupPlacement::GetCenterOnCursorPlacement(bool in_movable, const glm::vec2& in_alignment)
	{
		PopupPlacement result;
		result.placement_type = PopupPlacementType::CursorPosition;
		result.movable = in_movable;
		result.alignment = in_alignment;
		return result;
	}

	PopupPlacement PopupPlacement::GetCenterOnPositionPlacement(const glm::vec2& in_position, bool in_movable, const glm::vec2& in_alignment)
	{
		PopupPlacement result;
		result.placement_type = PopupPlacementType::FreePosition;
		result.movable = in_movable;
		result.alignment = in_alignment;
		result.position = in_position;
		return result;
	}

	namespace details
	{
		PopupState ImGuiPopupBase::GetPopupState() const
		{
			return popup_state;
		}

		void ImGuiPopupBase::Close()
		{
			if (popup_state == PopupState::Closed)
				return;

			if (popup_state == PopupState::Opened)
				ImGui::CloseCurrentPopup();

			popup_name = {};
			popup_state = PopupState::Closed;
		}

		int ImGuiPopupBase::GetPopupFlags() const
		{
			return ImGuiWindowFlags_AlwaysAutoResize;
		}

		bool ImGuiPopupBase::DoOpen(std::string in_popup_name, const PopupPlacement& in_placement)
		{
			if (popup_state != PopupState::Closed)
				return false;

			popup_name = std::move(in_popup_name);
			popup_placement = in_placement;
			popup_state = PopupState::Opening;
			return true;
		}

		void ImGuiPopupBase::PrepareModalPlacement()
		{
			if (popup_state == PopupState::Opening)
			{
				ImGui::OpenPopup(popup_name.c_str(), ImGuiPopupFlags_MouseButtonRight);

				ImVec2 position = { 0.0f, 0.0f };
				ImVec2 alignment = { popup_placement.alignment.x, popup_placement.alignment.y };

				if (popup_placement.placement_type == PopupPlacementType::ScreenCenter)
					position = ImGui::GetMainViewport()->GetCenter();
				else if (popup_placement.placement_type == PopupPlacementType::CursorPosition)
					position = ImGui::GetMousePos();
				else if (popup_placement.placement_type == PopupPlacementType::FreePosition)
					position = { popup_placement.position.x, popup_placement.position.y };

				ImGui::SetNextWindowPos(position, ImGuiCond_Always, alignment);

				popup_state = PopupState::Opened;
			}
		}

	}; // namespace details

}; // namespace chaos