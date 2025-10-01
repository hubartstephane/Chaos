#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiInputActionObject::ImGuiInputActionObject()
	{
		imgui_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	}

	void ImGuiInputActionObject::OnDrawImGuiContent(Window * window)
	{
		class OnQueryInputActionEnumerator : public InputActionEnumerator
		{
		public:

			using InputActionEnumerator::InputActionEnumerator;

			virtual bool CheckAndProcess(InputRequestBase const & in_request, char const * in_title, bool in_enabled, LightweightFunction<void()> in_key_action) override
			{
				char buffer[256];

				if (KeyRequest const* key_request = auto_cast(&in_request))
				{
					ImGui::TableNextRow();

					ImGui::BeginDisabled(!in_enabled);
					ImGui::TableSetColumnIndex(0); ImGui::Text("%s", key_request->key.GetName());

					char const* required_modifiers = EnumToString(key_request->required_modifiers, buffer, 256);
					ImGui::TableSetColumnIndex(1); ImGui::Text("%s", required_modifiers);

					char const* forbidden_modifiers = EnumToString(key_request->forbidden_modifiers, buffer, 256);
					ImGui::TableSetColumnIndex(2); ImGui::Text("%s", forbidden_modifiers);

					char const* action_mask = EnumToString(key_request->action_mask, buffer, 256);
					ImGui::TableSetColumnIndex(3); ImGui::Text("%s", action_mask);

					ImGui::TableSetColumnIndex(4); ImGui::Text("%s", in_title);
					ImGui::EndDisabled();
				}
				return false; // continue with following input receivers
			}
		};

		ImGuiTools::DrawImGuiTable("objects", {}, "Key", "Mandatory Mod.", "Forbidden Mod.", "Action", "Description")([&]()
		{
			auto process_function = [](InputReceiverInterface* in_input_receiver, InputDeviceInterface const * in_input_device) // XXX: mandatory to have a VARIABLE lambda so that the underlying DelegateTraverser's LightweightFunction does not point on a deleted object
			{
				OnQueryInputActionEnumerator action_enumerator(in_input_receiver);
				in_input_receiver->EnumerateInputActions(action_enumerator, EnumerateInputActionContext::OnQuery);
				return false; // pass through all receivers
			};

			DelegateInputReceiverTraverser traverser(process_function);
			window->TraverseInputReceiverFull(traverser); // include ImGuiWindowContext and WindowApplication in the traversal
		});
	}

}; // namespace chaos