#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiContentObjectBase::ImGuiContentObjectBase(std::string in_message, ImGuiButtonType in_buttons) :
		message(std::move(in_message)),
		buttons(in_buttons)
	{
		if (HasAnyFlags(in_buttons, ImGuiButtonType::Close))
			SetImGuiObjectFlags(GetImGuiObjectFlags() | chaos::ImGuiObjectFlags::CloseWithCross);
	}

	void ImGuiContentObjectBase::OnDrawImGuiContent(chaos::Window* window)
	{
		if (!chaos::StringTools::IsEmpty(message))
			ImGui::Text("%s", message.c_str());

		DisplayContent();
		DisplayButtons();
	}

	void ImGuiContentObjectBase::OnCloseWithCross()
	{
		if (HasAnyFlags(buttons, ImGuiButtonType::Close))
			CallNotification(ImGuiButtonType::Close);
	}

	void ImGuiContentObjectBase::DisplayContent()
	{
	}

	void ImGuiContentObjectBase::CallNotification(ImGuiButtonType button_type)
	{
	}

	void ImGuiContentObjectBase::DisplayButtons()
	{
		auto DrawButton = [this](ImGuiButtonType button_type, char const* button_title, bool request_closing)
		{
			if (HasAnyFlags(buttons, button_type))
			{
				if (ImGui::Button(button_title))
				{
					CallNotification(button_type);
					if (request_closing)
						RequestClosing();
				};
				ImGui::SameLine();
			}
		};

		DrawButton(ImGuiButtonType::Yes, "Yes", true);
		DrawButton(ImGuiButtonType::No, "No", true);
		DrawButton(ImGuiButtonType::Validate, "Validate", true);
		DrawButton(ImGuiButtonType::Cancel, "Cancel", true);
		DrawButton(ImGuiButtonType::Apply, "Apply", false);
		DrawButton(ImGuiButtonType::Close, "Close", true);
	}

}; // namespace chaos