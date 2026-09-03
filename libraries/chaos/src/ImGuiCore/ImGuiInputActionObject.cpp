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
		class ImGuiQueryInputActionProcessor : public InputActionProcessor
		{
		public:

			using InputActionProcessor::InputActionProcessor;

			virtual bool CheckAndProcess(InputConditionBase const & in_condition, char const * in_title, InputAction const & in_action) override
			{
				ImGui::TableNextRow();
				ImGui::BeginDisabled(!in_action.IsEnabled());

				ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

				InputConditionResult request_result = in_condition.Check({ input_receiver, input_device, consumption_cache });
				if (request_result == InputConditionResult::Invalid)
					color = { 1.0f, 0.0f, 0.0f, 1.0f };
				else if (request_result == InputConditionResult::Rejected)
					color = { 0.0f, 1.0f, 1.0f, 1.0f };

				int column_index = 0;
				ImGui::TableSetColumnIndex(column_index++); ImGui::TextColored(color, "%s", in_title);

				ImGui::TableSetColumnIndex(column_index++);
				if (char const * receiver_name = input_receiver->GetInputReceiverName())
					ImGui::TextColored(color, "%s", receiver_name);

				InputConditionDebugInfoStorage debug_info_storage;
				char const* debug_info = in_condition.GetDebugInfo(debug_info_storage);
				ImGui::TableSetColumnIndex(column_index++); ImGui::TextColored(color, "%s", debug_info);
					
				ImGui::TableSetColumnIndex(column_index++);
				if (request_result == InputConditionResult::Invalid)
					ImGui::TextColored(color, "Invalid", in_title);
				else if (request_result == InputConditionResult::Rejected)
					ImGui::TextColored(color, "Rejected", in_title);

				ImGui::TableSetColumnIndex(column_index++);
				if (!in_action.IsEnabled())
					ImGui::TextColored(color, "Disabled", in_title);

				ImGui::EndDisabled();

				return false; // continue with following input receivers
			}
		};

		class ImGuiQueryInputReceiverTraverser : public InputReceiverTraverser
		{
		public:

			using InputReceiverTraverser::InputReceiverTraverser;

		protected:

			virtual bool Process(InputReceiverInterface* in_input_receiver) override
			{
				ImGuiQueryInputActionProcessor action_processor(in_input_receiver, input_device, &consumption_cache);
				in_input_receiver->EnumerateInputActions(action_processor, EnumerateInputActionContext::OnQuery);
				return false; // pass through all receivers			
			}

		protected:

			InputConsumptionCache consumption_cache;
		};

		ImGuiTools::DrawImGuiTable("objects", {}, "Description", "Object", "Input", "Request Status", "Enabled")([&]()
		{
			KeyboardAndMouseDevice * keyboard_and_mouse_device = KeyboardAndMouseDevice::GetInstance();

			ImGuiQueryInputReceiverTraverser traverser(keyboard_and_mouse_device);
			window->TraverseInputReceiverFull(traverser); // include ImGuiWindowContext and WindowApplication in the traversal
		});
	}

}; // namespace chaos