#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	KeyState const * KeyboardAndMouseDevice::DoGetInputState(Key key) const
	{
		if (IsKeyboardInput(key))
		{
			size_t key_index = size_t(key) - size_t(Key::KEYBOARD_FIRST);
			return &keyboard_key_state[key_index];
		}
		if (IsMouseInput(key))
		{
			size_t key_index = size_t(key) - size_t(Key::MOUSE_FIRST);
			return &mouse_key_state[key_index];
		}
		return nullptr;
	}

	bool KeyboardAndMouseDevice::DoForAllKeys(ForAllKeysFunction func) const
	{
		for (size_t i = 0 ; i < keyboard_key_state.size() ; ++i)
			if (func(Key(i + size_t(Key::KEYBOARD_FIRST)), keyboard_key_state[i]))
				return true;
		for (size_t i = 0 ; i < mouse_key_state.size() ; ++i)
			if (func(Key(i + size_t(Key::MOUSE_FIRST)), mouse_key_state[i]))
				return true;
		return false;
	}

	void KeyboardAndMouseDevice::SetKeyValue(Key key, bool value)
	{
		if (IsKeyboardInput(key))
		{
			size_t key_index = size_t(key) - size_t(Key::KEYBOARD_FIRST);
			if (key_index >= 0 && key_index < keyboard_key_state.size())
				keyboard_key_state[key_index].SetValue(value);
		}
		else if (IsMouseInput(key))
		{
			size_t key_index = size_t(key) - size_t(Key::MOUSE_FIRST);
			if (key_index >= 0 && key_index < mouse_key_state.size())
				mouse_key_state[key_index].SetValue(value);
		}
	}

}; // namespace chaos
