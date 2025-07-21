#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void KeyboardAndMouseState::SetKeyValue(Key key, bool value)
	{
		KeyboardAndMouseState * singleton = KeyboardAndMouseState::GetInstance();

		if (key.IsKeyboardKey())
		{
			if (key != KeyboardButton::UNKNOWN)
			{
				int key_index = int(key.GetKeyboardButton());
				if (key_index >= 0 && key_index < singleton->keyboard_state.size())
					singleton->keyboard_state[key_index].SetValue(value);
			}
		}
		else if (key.IsMouseKey())
		{
			if (key != MouseButton::UNKNOWN)
			{
				int key_index = int(key.GetMouseButton());
				if (key_index >= 0 && key_index < singleton->mouse_button_state.size())
					singleton->mouse_button_state[key_index].SetValue(value);
			}
		}
	}

	ButtonState const * KeyboardAndMouseState::GetKeyState(Key key)
	{
		KeyboardAndMouseState const * singleton = KeyboardAndMouseState::GetInstance();

		if (key.IsKeyboardKey())
		{
			if (key != KeyboardButton::UNKNOWN)
			{
				int key_index = int(key.GetKeyboardButton());
				if (key_index >= 0 && key_index < singleton->keyboard_state.size())
					return &singleton->keyboard_state[key_index];
			}
		}
		else if (key.IsMouseKey())
		{
			if (key != MouseButton::UNKNOWN)
			{
				int key_index = int(key.GetMouseButton());
				if (key_index >= 0 && key_index < singleton->mouse_button_state.size())
					return &singleton->mouse_button_state[key_index];
			}
		}
		return nullptr;
	}

}; // namespace chaos
