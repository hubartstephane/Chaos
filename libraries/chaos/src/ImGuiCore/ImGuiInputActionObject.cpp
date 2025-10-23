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

			OnQueryInputActionEnumerator(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache):
				InputActionEnumerator(in_input_receiver),
				input_device(in_input_device),
				consumption_cache(in_consumption_cache)
			{
			}



			virtual bool CheckAndProcess(InputRequestBase const & in_request, char const * in_title, bool in_enabled, LightweightFunction<void()> in_key_action) override
			{
				ImGui::TableNextRow();
				ImGui::BeginDisabled(!in_enabled);

				ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

				InputRequestResult request_result = in_request.Check(input_receiver, input_device, *consumption_cache);
				if (request_result == InputRequestResult::Invalid)
					color = { 1.0f, 0.0f, 0.0f, 1.0f };
				else if (request_result == InputRequestResult::Rejected)
					color = { 0.0f, 1.0f, 1.0f, 1.0f };

				InputRequestDebugInfo debug_info = in_request.GetDebugInfo();

				if (char const * receiver_name = input_receiver->GetInputReceiverName())
				{
					ImGui::TableSetColumnIndex(0); 
					ImGui::TextColored(color, "%s", receiver_name);
				}

				ImGui::TableSetColumnIndex(1); ImGui::TextColored(color, "%s", debug_info.input.c_str());
				ImGui::TableSetColumnIndex(2); ImGui::TextColored(color, "%s", debug_info.required_modifiers.c_str());
				ImGui::TableSetColumnIndex(3); ImGui::TextColored(color, "%s", debug_info.forbidden_modifiers.c_str());
				ImGui::TableSetColumnIndex(4); ImGui::TextColored(color, "%s", debug_info.action_type.c_str());
			
				if (!in_enabled)
				{
					ImGui::TableSetColumnIndex(5);
					ImGui::TextColored(color, "Disabled", in_title);
				}
					
				ImGui::TableSetColumnIndex(6);
				if (request_result == InputRequestResult::Invalid)
					ImGui::TextColored(color, "Invalid", in_title);
				else if (request_result == InputRequestResult::Rejected)
					ImGui::TextColored(color, "Rejected", in_title);

				ImGui::TableSetColumnIndex(7); ImGui::TextColored(color, "%s", in_title);

				ImGui::EndDisabled();

				return false; // continue with following input receivers
			}

		protected:

			/** the input device considered */
			InputDeviceInterface const* input_device = nullptr;
			/** the consumption cache */
			InputConsumptionCache* consumption_cache = nullptr;
		};

		ImGuiTools::DrawImGuiTable("objects", {}, "Object", "Input", "Mandatory Mod.", "Forbidden Mod.", "Action", "Enabled", "Request Status", "Description")([&]()
		{
			InputConsumptionCache consumption_cache;

			auto process_function = [&consumption_cache](InputReceiverInterface* in_input_receiver, InputDeviceInterface const * in_input_device) // XXX: mandatory to have a VARIABLE lambda so that the underlying DelegateTraverser's LightweightFunction does not point on a deleted object
			{
				OnQueryInputActionEnumerator action_enumerator(in_input_receiver, in_input_device, &consumption_cache);
				in_input_receiver->EnumerateInputActions(action_enumerator, EnumerateInputActionContext::OnQuery);
				return false; // pass through all receivers
			};

			DelegateInputReceiverTraverser traverser(process_function);
			window->TraverseInputReceiverFull(traverser); // include ImGuiWindowContext and WindowApplication in the traversal
		});
	}

}; // namespace chaos