#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	//
	// KeyboardAndMouseState
	//

	std::array<ButtonState, GLFW_KEY_LAST> KeyboardAndMouseState::keyboard_state;

	std::array<ButtonState, GLFW_MOUSE_BUTTON_LAST + 1> KeyboardAndMouseState::mouse_button_state;

	void KeyboardAndMouseState::SetKeyboardButtonState(KeyboardButton key, int action)
	{
		if (key != KeyboardButton::UNKNOWN)
		{
			int key_index = int(key);
			if (key_index >= 0 && key_index < keyboard_state.size())
				keyboard_state[key_index].SetValue(action == GLFW_PRESS || action == GLFW_REPEAT);
		}
	}

	void KeyboardAndMouseState::SetMouseButtonState(MouseButton key, int action)
	{
		if (key != MouseButton::UNKNOWN)
		{
			int key_index = int(key);
			if (key_index >= 0 && key_index < mouse_button_state.size())
				mouse_button_state[key_index].SetValue(action == GLFW_PRESS || action == GLFW_REPEAT);
		}
	}

	ButtonState const* KeyboardAndMouseState::GetKeyboardButtonState(KeyboardButton key)
	{
		if (key != KeyboardButton::UNKNOWN)
		{
			int key_index = int(key);
			if (key_index >= 0 && key_index < keyboard_state.size())
				return &keyboard_state[key_index];
		}
		return nullptr;
	}

	ButtonState const* KeyboardAndMouseState::GetMouseButtonState(MouseButton key)
	{
		if (key != MouseButton::UNKNOWN)
		{
			int key_index = int(key);
			if (key_index >= 0 && key_index < mouse_button_state.size())
				return &mouse_button_state[key_index];
		}
		return nullptr;
	}

	ButtonState const * KeyboardAndMouseState::GetKeyState(Key key)
	{
		if (key.IsKeyboardKey())
			return GetKeyboardButtonState(key.GetKeyboardButton());
		if (key.IsMouseKey())
			return GetMouseButtonState(key.GetMouseButton());
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
