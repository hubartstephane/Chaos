#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void InputConsumptionCache::SetConsumeAllInputs(bool in_value)
	{
		consume_all_inputs = in_value;
	}

	void InputConsumptionCache::Clear()
	{
		consumed_keys.clear();
		consumed_input1D.clear();
		consumed_input2D.clear();
		consume_all_inputs = false;
	}

	bool InputConsumptionCache::TryConsumeInput(Key const& in_key, InputDeviceInterface const* in_input_device) // check whether the key is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle key
		result &= DoTryConsumeInput(in_key, in_input_device);
		// handle 'virtual related' keys/inputs
		EnumerateVirtualKeys([this, in_key, &result, in_input_device](Key key, Input1D input)
		{
			if (key != in_key)
				return false;
			result &= DoTryConsumeInput(input, in_input_device);
			return true;
		});

		return (consume_all_inputs)? 
			false:
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(Input1D in_input, InputDeviceInterface const* in_input_device) // check whether the axis is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle axis
		result &= DoTryConsumeInput(in_input, in_input_device);

		// handle 'virtual related' keys/inputs
		EnumerateVirtualKeys([this, in_input, &result, in_input_device](Key key, Input1D input)
		{
			if (input != in_input)
				return false;
			result &= DoTryConsumeInput(key, in_input_device);
			return true;
		});

		EnumerateVirtualInputs([this, in_input, &result, in_input_device](Input1D input1D_x, Input1D input1D_y, Input2D input2D)
		{
			if (in_input != input1D_x && in_input != input1D_y)
				return false;
			result &= DoTryConsumeInput(input2D, in_input_device);
			return true;
		});

		return (consume_all_inputs) ?
			false :
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(Input2D in_input, InputDeviceInterface const* in_input_device) // check whether the stick is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle stick
		result &= DoTryConsumeInput(in_input, in_input_device);

		// handle 'virtual related' inputs
		EnumerateVirtualInputs([this, in_input, &result, in_input_device](Input1D input1D_x, Input1D input1D_y, Input2D input2D)
		{
			if (in_input != input2D)
				return false;
			result &= DoTryConsumeInput(input1D_x, in_input_device);
			result &= DoTryConsumeInput(input1D_y, in_input_device);
			return true;
		});

		return (consume_all_inputs) ?
			false :
			result;
	}

	bool InputConsumptionCache::EnumerateVirtualKeys(VirtualKeyEnumerationFunction func)
	{
		if (func(Key::GAMEPAD_LEFT_TRIGGER, Input1D::GAMEPAD_LEFT_TRIGGER))
			return true;
		if (func(Key::GAMEPAD_RIGHT_TRIGGER, Input1D::GAMEPAD_RIGHT_TRIGGER))
			return true;
		return false;
	}

	bool InputConsumptionCache::EnumerateVirtualInputs(VirtualInputEnumerationFunction func)
	{
		if (func(Input1D::GAMEPAD_LEFT_AXIS_X, Input1D::GAMEPAD_LEFT_AXIS_Y, Input2D::GAMEPAD_LEFT_STICK))
			return true;
		if (func(Input1D::GAMEPAD_RIGHT_AXIS_X, Input1D::GAMEPAD_RIGHT_AXIS_Y, Input2D::GAMEPAD_RIGHT_STICK))
			return true;
		if (func(Input1D::MOUSE_WHEEL_X, Input1D::MOUSE_WHEEL_Y, Input2D::MOUSE_WHEEL))
			return true;

		return false;
	}

	bool InputConsumptionCache::DoTryConsumeInput(Key in_key, InputDeviceInterface const* in_input_device)
	{
		KeyState const* state = in_input_device->GetInputState(in_key);
		if (state == nullptr)
			return false;
		return consumed_keys.insert(std::make_pair(in_key, state)).second; // insert returns a pair. second element indicates whether the element has effectively been inserted
	}

	bool InputConsumptionCache::DoTryConsumeInput(Input1D in_input, InputDeviceInterface const* in_input_device)
	{
		Input1DState const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return false;
		return consumed_input1D.insert(std::make_pair(in_input, state)).second; // insert returns a pair. second element indicates whether the element has effectively been inserted
	}

	bool InputConsumptionCache::DoTryConsumeInput(Input2D in_input, InputDeviceInterface const* in_input_device)
	{
		Input2DState const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return false;
		return consumed_input2D.insert(std::make_pair(in_input, state)).second; // insert returns a pair. second element indicates whether the element has effectively been inserted
	}

}; // namespace chaos
