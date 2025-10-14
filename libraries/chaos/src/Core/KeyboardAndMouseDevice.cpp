#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	KeyState const * KeyboardAndMouseDevice::DoGetInputState(Key input) const
	{
		if (IsKeyboardInput(input))
		{
			size_t input_index = size_t(input) - size_t(Key::KEYBOARD_FIRST);
			return &keyboard_key_state[input_index];
		}
		if (IsMouseInput(input))
		{
			size_t input_index = size_t(input) - size_t(Key::MOUSE_FIRST);
			return &mouse_key_state[input_index];
		}
		return nullptr;
	}

	Input1DState const* KeyboardAndMouseDevice::DoGetInputState(Input1D input) const
	{
		if (IsMouseInput(input))
		{
			size_t input_index = size_t(input) - size_t(Input1D::MOUSE_FIRST);
			return &mouse_input1D_state[input_index];
		}
		return nullptr;
	}
	
	Input2DState const* KeyboardAndMouseDevice::DoGetInputState(Input2D input) const
	{
		if (IsMouseInput(input))
		{
			size_t input_index = size_t(input) - size_t(Input2D::MOUSE_FIRST);
			return &mouse_input2D_state[input_index];
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
	
	bool KeyboardAndMouseDevice::DoForAllInput1D(ForAllInput1DFunction func) const
	{
		for (size_t i = 0; i < mouse_input1D_state.size(); ++i)
			if (func(Input1D(i + size_t(Input1D::MOUSE_FIRST)), mouse_input1D_state[i]))
				return true;
		return false;
	}
	
	bool KeyboardAndMouseDevice::DoForAllInput2D(ForAllInput2DFunction func) const
	{
		for (size_t i = 0; i < mouse_input2D_state.size(); ++i)
			if (func(Input2D(i + size_t(Input2D::MOUSE_FIRST)), mouse_input2D_state[i]))
				return true;
		return false;
	}

	void KeyboardAndMouseDevice::SetInputValue(Key input, bool value)
	{
		if (IsKeyboardInput(input))
		{
			size_t input_index = size_t(input) - size_t(Key::KEYBOARD_FIRST);
			if (input_index >= 0 && input_index < keyboard_key_state.size())
				keyboard_key_state[input_index].SetValue(value);
		}
		else if (IsMouseInput(input))
		{
			size_t input_index = size_t(input) - size_t(Key::MOUSE_FIRST);
			if (input_index >= 0 && input_index < mouse_key_state.size())
				mouse_key_state[input_index].SetValue(value);
		}
	}

	void KeyboardAndMouseDevice::SetInputValue(Input1D input, float value)
	{
		if (IsMouseInput(input))
		{
			size_t input_index = size_t(input) - size_t(Input1D::MOUSE_FIRST);
			if (input_index >= 0 && input_index < mouse_input1D_state.size())
				mouse_input1D_state[input_index].SetValue(value, 0.0f, InputDeviceType::MOUSE);
		}
	}

	void KeyboardAndMouseDevice::SetInputValue(Input2D input, glm::vec2 const & value)
	{
		if (IsMouseInput(input))
		{
			size_t input_index = size_t(input) - size_t(Input2D::MOUSE_FIRST);
			if (input_index >= 0 && input_index < mouse_input2D_state.size())
				mouse_input2D_state[input_index].SetValue(value, 0.0f, InputDeviceType::MOUSE);
		}
	}

}; // namespace chaos
