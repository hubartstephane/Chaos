#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ImGuiInputStateObject::OnDrawImGuiContent(Window * window)
	{
		auto DisplayKeyStates = [](char const * title, char const * table_title, auto key_min, auto key_max, auto GetKeyStateFunc)
		{
			ImGui::SeparatorText(title);

			WithImGuiInputTable(table_title, [key_min, key_max, &GetKeyStateFunc]()
			{
				for (auto key = key_min ; key <= key_max ; key = (decltype(key))(int(key) + 1)) // not the better way to increment an enum
				{
					if (ButtonState const * button_state = GetKeyStateFunc(key))
					{
						// do not bother display keys up for more than 10s
						if (!button_state->GetValue() && button_state->GetSameValueTimer() > 10.0f)
							continue;

						DisplayImGuiKeyInfo(key, *button_state);
					}
				}
			});
		};

		DisplayKeyStates("Mouse", "Mouse Table", MouseButton::BUTTON_1, MouseButton::BUTTON_8, &KeyboardState::GetMouseButtonState);
		DisplayKeyStates("Keyboard", "Keyboard Table", KeyboardButton::SPACE, KeyboardButton::MENU, &KeyboardState::GetKeyboardButtonState);
	}

}; // namespace chaos