#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiMessageObject::ImGuiMessageObject(std::string in_message) :
		message(std::move(in_message))
	{
	}

	void ImGuiMessageObject::OnDrawImGuiContent(Window* window)
	{
		ImGui::Text("%s", message.c_str());
		if (ImGui::Button("Close"))
			RequestClosing();
	}

}; // namespace chaos