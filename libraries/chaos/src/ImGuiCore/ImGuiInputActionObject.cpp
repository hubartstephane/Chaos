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

			~ImGuiQueryInputActionProcessor()
			{
				if (table_opened)
				{
					ImGui::EndTable();
					ImGui::Dummy({0.0f, 40.0f});
				}
			}

			virtual bool CheckAndProcess(InputConditionBase const & in_condition, char const * in_title, InputAction const & in_action) override
			{
				if (!table_opened)
				{
					char const* receiver_name = input_receiver->GetInputReceiverName();
					if (receiver_name == nullptr)
						receiver_name = "unnamed";

					ImGui::TableNextColumn();
					ImGui::Text("%s", receiver_name);
					ImGui::TableNextColumn();

					table_opened = ImGuiTools::OpenTable("objects", {}, "Description", "Input", "Request Status");
					if (!table_opened)
						return false;
				}

				ImGui::TableNextRow();
				ImGui::BeginDisabled(!in_action.IsEnabled());

				ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

				InputConditionResult request_result = in_condition.Check({ input_receiver, input_device, consumption_system });
				if (request_result.type == InputConditionResultType::Invalid)
					color = { 1.0f, 0.0f, 0.0f, 1.0f };
				else if (request_result.type == InputConditionResultType::Rejected)
					color = { 0.0f, 1.0f, 1.0f, 1.0f };

				int column_index = 0;

				ImGui::TableSetColumnIndex(column_index++); ImGui::TextColored(color, "%s", in_title);

				InputConditionDebugInfoStorage debug_info_storage;
				char const* debug_info = in_condition.GetDebugInfo(debug_info_storage);
				ImGui::TableSetColumnIndex(column_index++); ImGui::TextColored(color, "%s", debug_info);
					
				ImGui::TableSetColumnIndex(column_index++);
				if (request_result.type == InputConditionResultType::Invalid)
					ImGui::TextColored(color, "Invalid");
				else if (request_result.type == InputConditionResultType::Rejected)
					ImGui::TextColored(color, "Rejected");

				ImGui::EndDisabled();

				return false; // continue with following input receivers
			}

		protected:

			bool table_opened = false;
		};

		class ImGuiQueryInputReceiverTraverser : public InputReceiverTraverser
		{
		public:

			using InputReceiverTraverser::InputReceiverTraverser;

		protected:

			virtual bool Process(InputReceiverInterface* in_input_receiver) override
			{
				ImGui::TableNextRow();
				ImGui::PushID(in_input_receiver);
				{
					ImGuiQueryInputActionProcessor action_processor(in_input_receiver, input_device, &consumption_system);
					in_input_receiver->EnumerateInputActions(action_processor, EnumerateInputActionContext::OnQuery);
				}
				ImGui::PopID();
				return false; // pass through all receivers			
			}

		protected:

			InputConsumptionSystem consumption_system;
		};

		DrawImGuiTable("objects", {}, "Input Receiver", "Actions")([&]()
		{
			KeyboardAndMouseDevice* keyboard_and_mouse_device = KeyboardAndMouseDevice::GetInstance();
			ImGuiQueryInputReceiverTraverser traverser(keyboard_and_mouse_device);
			window->TraverseInputReceiverFull(traverser); // include ImGuiWindowContext and WindowApplication in the traversal		
		});
	}

}; // namespace chaos