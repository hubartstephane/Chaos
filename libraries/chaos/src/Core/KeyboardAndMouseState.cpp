#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	//
	// KeyboardAndMouseState
	//

	std::array<ButtonState, GLFW_KEY_LAST> KeyboardAndMouseState::keyboard_state;

	std::array<ButtonState, GLFW_MOUSE_BUTTON_LAST + 1> KeyboardAndMouseState::mouse_button_state;

	void KeyboardAndMouseState::SetKeyState(Key key, bool value)
	{
		if (key.IsKeyboardKey())
		{
			if (key != KeyboardButton::UNKNOWN)
			{
				int key_index = int(key.GetKeyboardButton());
				if (key_index >= 0 && key_index < keyboard_state.size())
					keyboard_state[key_index].SetValue(value);
			}
		}
		else if (key.IsMouseKey())
		{
			if (key != MouseButton::UNKNOWN)
			{
				int key_index = int(key.GetMouseButton());
				if (key_index >= 0 && key_index < mouse_button_state.size())
					mouse_button_state[key_index].SetValue(value);
			}
		}
	}

	ButtonState const * KeyboardAndMouseState::GetKeyState(Key key)
	{
		if (key.IsKeyboardKey())
		{
			if (key != KeyboardButton::UNKNOWN)
			{
				int key_index = int(key.GetKeyboardButton());
				if (key_index >= 0 && key_index < keyboard_state.size())
					return &keyboard_state[key_index];
			}
		}
		else if (key.IsMouseKey())
		{
			if (key != MouseButton::UNKNOWN)
			{
				int key_index = int(key.GetMouseButton());
				if (key_index >= 0 && key_index < mouse_button_state.size())
					return &mouse_button_state[key_index];
			}
		}
		return nullptr;
	}

	void KeyboardAndMouseState::UpdateKeysTimerAccumulation(float delta_time)
	{
		for (ButtonState& button : keyboard_state)
			button.UpdateTimerAccumulation(delta_time);
		for (ButtonState& button : mouse_button_state)
			button.UpdateTimerAccumulation(delta_time);
	}

}; // namespace chaos
