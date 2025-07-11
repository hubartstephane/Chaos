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
				ImGui::TableNextRow();

				ImGui::BeginDisabled(!in_enabled);
				ImGui::TableSetColumnIndex(0); ImGui::Text("%s", EnumToString(in_request.key));
				//ImGui::TableSetColumnIndex(1); ImGui::Text("%s", EnumToString(in_request.required_modifiers));
				//ImGui::TableSetColumnIndex(2); ImGui::Text("%s", EnumToString(in_request.action_mask));


				ImGui::TableSetColumnIndex(1); ImGui::Text("modif");
				ImGui::TableSetColumnIndex(2); ImGui::Text("mask");

				ImGui::TableSetColumnIndex(3); ImGui::Text("%s", in_title);
				ImGui::EndDisabled();

				return false;
			}
		};

		if (ImGui::BeginTable("objects", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Key");
			ImGui::TableSetupColumn("Modifiers");
			ImGui::TableSetupColumn("Action");
			ImGui::TableSetupColumn("Description");
			ImGui::TableHeadersRow();

			ImGuiKeyActionEnumerator action_enumerator;
			window->DispatchEventToHierarchy([&action_enumerator](InputEventReceiverInterface * in_event_receiver)
			{
				in_event_receiver->EnumerateKeyActions(action_enumerator);
				return false;
			});

			ImGui::EndTable();
		}
	}

}; // namespace chaos