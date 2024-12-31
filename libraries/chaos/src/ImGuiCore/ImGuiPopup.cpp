#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumTools::EnumBitmaskMetaData<ImGuiPopupFlags> const ImGuiPopupFlags_bitmask_metadata =
	{
		{ImGuiPopupFlags::NonModal, ImGuiPopupFlags::Modal},
		{ImGuiPopupFlags::NonModal, ImGuiPopupFlags::CloseWithCross},
		{ImGuiPopupFlags::NonModal, ImGuiPopupFlags::CloseWithEscape}
	};

	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(ImGuiPopupFlags, &ImGuiPopupFlags_bitmask_metadata, CHAOS_API);

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

		bool ImGuiPopupBase::DoOpen(std::string in_popup_name, ImGuiPopupFlags in_popup_flags, const ImGuiWindowPlacement& in_placement)
		{
			assert(AreValidFlags(in_popup_flags));

			if (popup_state != ImGuiPopupState::Closed)
				return false;

			popup_name = std::move(in_popup_name);
			popup_placement = in_placement;
			popup_flags = in_popup_flags;
			popup_state = ImGuiPopupState::Opening;
			return true;
		}

	}; // namespace details

}; // namespace chaos