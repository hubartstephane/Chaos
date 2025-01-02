#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	ImGuiYesNoObject::ImGuiYesNoObject(std::string in_message, std::function<void(bool)> in_response_function) :
		message(std::move(in_message)),
		response_function(std::move(in_response_function))
	{
	}

	void ImGuiYesNoObject::OnDrawImGuiContent(Window* window)
	{
		ImGui::Text("%s", message.c_str());
		if (ImGui::Button("Yes"))
		{
			RequestClosing();
			response_function(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			RequestClosing();
			response_function(false);
		}
	}

}; // namespace chaos