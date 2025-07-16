#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiInputStateObject::OnDrawImGuiContent(Window * window)
	{
		auto DisplayKeyStates = [](char const * title, char const * table_title, auto key_min, auto key_max, auto GetKeyStateFunc)
		{
			ImGui::SeparatorText(title);

			if (ImGui::BeginTable(table_title, 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg))
			{
				ImGui::TableSetupColumn("Key", 0);
				ImGui::TableSetupColumn("State", 0);
				ImGui::TableSetupColumn("Repeat Timer", 0);
				ImGui::TableHeadersRow();

				for (auto key = key_min ; key <= key_max ; key = (decltype(key))(int(key) + 1)) // not the better way to increment an enum
				{
					if (ButtonState const * button_state = GetKeyStateFunc(key))
					{
						// do not bother display keys up for more than 10s
						if (!button_state->GetValue() && button_state->GetSameValueTimer() > 10.0f)
							continue;

						if (char const * key_name = EnumToString(key)) // there are some hole in keys enum. Do not display them
						{
							bool pressed = button_state->GetValue();

							ImVec4 color = (pressed)? 
								ImVec4(1.0f, 0.0f, 0.0f, 1.0f):
								ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

							ImGui::PushID(int(key));

							ImGui::TableNextColumn();
							ImGui::TextColored(color, "%s", key_name);

							ImGui::TableNextColumn();
							ImGui::TextColored(color, "%s", pressed? "pressed" : "released");

							ImGui::TableNextColumn();
							ImGui::TextColored(color, "%f", button_state->GetSameValueTimer());

							ImGui::PopID();
						}
					}
				}
				ImGui::EndTable();
			}
		};

		DisplayKeyStates("Mouse", "Mouse Table", MouseButton::BUTTON_1, MouseButton::BUTTON_8, &KeyboardState::GetMouseButtonState);
		DisplayKeyStates("Keyboard", "Keyboard Table", KeyboardButton::SPACE, KeyboardButton::MENU, &KeyboardState::GetKeyboardButtonState);
	}

}; // namespace chaos