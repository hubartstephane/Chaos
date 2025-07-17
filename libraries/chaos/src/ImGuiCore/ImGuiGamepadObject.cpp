#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiGamepadObject::OnDrawImGuiContent(Window * window)
	{
		GameApplication const * game_application = Application::GetConstInstance();
		if (game_application == nullptr)
			return;

		Game const * game = game_application->GetGame();
		if (game == nullptr)
			return;

		GamepadManager const * gamepad_manager = game->GetGamepadManager();
		if (gamepad_manager == nullptr)
			return;

		size_t gamepad_count = gamepad_manager->GetPhysicalGamepadCount();

		for (size_t i = 0 ; i < gamepad_count ; ++i)
		{
			PhysicalGamepad const * physical_gamepad = gamepad_manager->GetPhysicalGamepad(i);
			if (physical_gamepad == nullptr)
				continue;

			if (!physical_gamepad->IsPresent())
				continue;

			size_t button_count = physical_gamepad->GetButtonCount();
			size_t axis_count   = physical_gamepad->GetAxisCount();

			ImGui::PushID(int(i));

			ImGui::SeparatorText("Gamepad");

			ImGui::Dummy({0.0f, 20.0f});
			if (ImGui::BeginTable("Gamepad State", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg))
			{
				ImGui::TableNextColumn();
				ImGui::Text("Index");
				ImGui::TableNextColumn();
				ImGui::Text("%d", physical_gamepad->GetGamepadIndex());

				ImGui::TableNextColumn();
				ImGui::Text("Allocated");
				ImGui::TableNextColumn();
				ImGui::Text("%s", physical_gamepad->IsAllocated()? "true" : "false");

				ImGui::TableNextColumn();
				ImGui::Text("Buttons");
				ImGui::TableNextColumn();
				ImGui::Text("%d", button_count);

				ImGui::TableNextColumn();
				ImGui::Text("Axes");
				ImGui::TableNextColumn();
				ImGui::Text("%d", axis_count);

				ImGui::EndTable();
			}
			if (button_count > 0)
			{
				ImGui::Dummy({0.0f, 20.0f});

				WithImGuiInputTable("buttons", [physical_gamepad]()
				{
					for (GamepadButton key = GamepadButton::A ; key <= GamepadButton::DPAD_LEFT ; key = (decltype(key))(int(key) + 1)) // not the better way to increment an enum
						DisplayImGuiKeyInfo(key, {});
					DisplayImGuiKeyInfo(GamepadButton::LEFT_TRIGGER, {});
					DisplayImGuiKeyInfo(GamepadButton::RIGHT_TRIGGER, {});
				});
			}



			ImGui::PopID();
		}




		


#if 0

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
#endif
	}

}; // namespace chaos