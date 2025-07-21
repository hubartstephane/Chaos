#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	KeyState const * KeyboardAndMouseState::DoGetKeyState(Key key) const
	{
		if (!key.IsValid())
			return nullptr;
		if (key.IsKeyboardKey())
			return &keyboard_state[int(key.GetKeyboardButton())];
		if (key.IsMouseKey())
			return &mouse_button_state[int(key.GetMouseButton())];
		return nullptr;
	}

	bool KeyboardAndMouseState::DoForAllKeys(LightweightFunction<bool(Key, KeyState const &)> func) const
	{
		for (int i = 0 ; i < keyboard_state.size() ; ++i)
			if (func(KeyboardButton(i), keyboard_state[i]))
				return true;
		for (int i = 0 ; i < mouse_button_state.size() ; ++i)
			if (func(MouseButton(i), mouse_button_state[i]))
				return true;
		return false;
	}

	void KeyboardAndMouseState::SetKeyValue(Key key, bool value)
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

}; // namespace chaos
