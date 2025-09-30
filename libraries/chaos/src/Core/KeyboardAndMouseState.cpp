#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	KeyState const * KeyboardAndMouseState::DoGetInputState(Key key) const
	{
		if (!key.IsValid())
			return nullptr;
		if (key.IsKeyboardKey())
			return &keyboard_key_state[int(key.GetKeyboardButton())];
		if (key.IsMouseKey())
			return &mouse_key_state[int(key.GetMouseButton())];
		return nullptr;
	}

	bool KeyboardAndMouseState::DoForAllKeys(ForAllKeysFunction func) const
	{
		for (int i = 0 ; i < keyboard_key_state.size() ; ++i)
			if (func(KeyboardButton(i), keyboard_key_state[i]))
				return true;
		for (int i = 0 ; i < mouse_key_state.size() ; ++i)
			if (func(MouseButton(i), mouse_key_state[i]))
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
				if (key_index >= 0 && key_index < keyboard_key_state.size())
					keyboard_key_state[key_index].SetValue(value);
			}
		}
		else if (key.IsMouseKey())
		{
			if (key != MouseButton::UNKNOWN)
			{
				int key_index = int(key.GetMouseButton());
				if (key_index >= 0 && key_index < mouse_key_state.size())
					mouse_key_state[key_index].SetValue(value);
			}
		}
	}

}; // namespace chaos
