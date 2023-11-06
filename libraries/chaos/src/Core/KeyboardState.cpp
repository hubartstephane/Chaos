#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	//
	// KeyboardState
	//

	std::array<ButtonState, GLFW_KEY_LAST> KeyboardState::keyboard_state;

	std::array<ButtonState, GLFW_MOUSE_BUTTON_LAST + 1> KeyboardState::mouse_button_state;

	void KeyboardState::SetKeyboardButtonState(KeyboardButton key, int action)
	{
		if (key != KeyboardButton::UNKNOWN)
		{
			int raw_value = int(key);
			if (raw_value >= 0 && raw_value < keyboard_state.size())
				keyboard_state[raw_value].SetValue(action == GLFW_PRESS || action == GLFW_REPEAT);
		}
	}

	void KeyboardState::SetMouseButtonState(MouseButton key, int action)
	{
		if (key != MouseButton::UNKNOWN)
		{
			int raw_value = int(key);
			if (raw_value >= 0 && raw_value < mouse_button_state.size())
				mouse_button_state[raw_value].SetValue(action == GLFW_PRESS || action == GLFW_REPEAT);
		}
	}

	ButtonState const* KeyboardState::GetKeyboardButtonState(KeyboardButton key)
	{
		if (key != KeyboardButton::UNKNOWN)
		{
			int raw_value = int(key);
			if (raw_value >= 0 && raw_value < keyboard_state.size())
				return &keyboard_state[raw_value];
		}
		return nullptr;
	}

	ButtonState const* KeyboardState::GetMouseButtonState(MouseButton key)
	{
		if (key != MouseButton::UNKNOWN)
		{
			int raw_value = int(key);
			if (raw_value >= 0 && raw_value < mouse_button_state.size())
				return &mouse_button_state[raw_value];
		}
		return nullptr;
	}

	ButtonState const * KeyboardState::GetKeyState(Key key)
	{
		if (key.IsKeyboardKey())
			return GetKeyboardButtonState(key.GetKeyboardButton());
		if (key.IsMouseKey())
			return GetMouseButtonState(key.GetMouseButton());
		return nullptr;
	}

	void KeyboardState::UpdateKeyStates(float delta_time)
	{
		for (ButtonState& button : keyboard_state)
			button.UpdateTimerAccumulation(delta_time);
		for (ButtonState& button : mouse_button_state)
			button.UpdateTimerAccumulation(delta_time);
	}

}; // namespace chaos
