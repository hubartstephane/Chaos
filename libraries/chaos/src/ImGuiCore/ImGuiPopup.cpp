#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
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