#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiHelpObject::ImGuiHelpObject()
	{
		imgui_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	}

	void ImGuiHelpObject::OnDrawImGuiContent(Window * window)
	{
		class ImGuiKeyActionEnumerator : public KeyActionEnumerator
		{
		public:

			using KeyActionEnumerator::KeyActionEnumerator;

			virtual bool operator () (KeyRequest const & in_request, char const * in_title, bool in_enabled, LightweightFunction<void()> in_key_action) override
			{
				char buffer[256];

				ImGui::TableNextRow();

				ImGui::BeginDisabled(!in_enabled);
				ImGui::TableSetColumnIndex(0); ImGui::Text("%s", in_request.key.GetName());

				char const* required_modifiers = EnumToString(in_request.required_modifiers, buffer, 256);
				ImGui::TableSetColumnIndex(1); ImGui::Text("%s", required_modifiers);

				char const* forbidden_modifiers = EnumToString(in_request.forbidden_modifiers, buffer, 256);
				ImGui::TableSetColumnIndex(2); ImGui::Text("%s", forbidden_modifiers);
				
				char const* action_mask = EnumToString(in_request.action_mask, buffer, 256);
				ImGui::TableSetColumnIndex(3); ImGui::Text("%s", action_mask);

				ImGui::TableSetColumnIndex(4); ImGui::Text("%s", in_title);
				ImGui::EndDisabled();

				return false;
			}
		};

		ImGuiTools::DrawImGuiTable("objects", {}, "Key", "Mandatory Mod.", "Forbidden Mod.", "Action", "Description")([&]()
		{
			ImGuiKeyActionEnumerator action_enumerator;
			window->TraverseInputEventReceiverHierarchy([&action_enumerator](InputEventReceiverInterface * in_event_receiver)
			{
				in_event_receiver->EnumerateKeyActions(action_enumerator);
				return false;
			});
		});
	}

}; // namespace chaos