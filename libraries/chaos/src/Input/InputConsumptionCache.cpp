#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void InputConsumptionCache::SetConsumeAllInputs(InputReceiverInterface const* in_input_receiver, bool in_value)
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

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, Key const& in_key, InputDeviceInterface const* in_input_device) // check whether the key is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle key
		result &= DoTryConsumeInput(in_input_receiver, in_key, in_input_device);
		// handle 'virtual related' keys/inputs
		EnumerateVirtualKeys([this, in_key, &result, in_input_device, in_input_receiver](Key key, Input1D input)
		{
			if (key != in_key)
				return false;
			result &= DoTryConsumeInput(in_input_receiver, input, in_input_device);
			return true;
		});

		return (consume_all_inputs)? 
			false:
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, Input1D in_input, InputDeviceInterface const* in_input_device) // check whether the axis is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle axis
		result &= DoTryConsumeInput(in_input_receiver, in_input, in_input_device);

		// handle 'virtual related' keys/inputs
		EnumerateVirtualKeys([this, in_input, &result, in_input_device, in_input_receiver](Key key, Input1D input)
		{
			if (input != in_input)
				return false;
			result &= DoTryConsumeInput(in_input_receiver, key, in_input_device);
			return true;
		});

		EnumerateVirtualInputs([this, in_input, &result, in_input_device, in_input_receiver](Input1D input1D_x, Input1D input1D_y, Input2D input2D)
		{
			if (in_input != input1D_x && in_input != input1D_y)
				return false;
			result &= DoTryConsumeInput(in_input_receiver, input2D, in_input_device);
			return true;
		});

		return (consume_all_inputs) ?
			false :
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, Input2D in_input, InputDeviceInterface const* in_input_device) // check whether the stick is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle stick
		result &= DoTryConsumeInput(in_input_receiver, in_input, in_input_device);

		// handle 'virtual related' inputs
		EnumerateVirtualInputs([this, in_input, &result, in_input_device, in_input_receiver](Input1D input1D_x, Input1D input1D_y, Input2D input2D)
		{
			if (in_input != input2D)
				return false;
			result &= DoTryConsumeInput(in_input_receiver, input1D_x, in_input_device);
			result &= DoTryConsumeInput(in_input_receiver, input1D_y, in_input_device);
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

	template<typename CONTAINER_TYPE, typename INPUT_TYPE, typename STATE_TYPE>
	bool TryInsertConsumedInput(CONTAINER_TYPE & inout_consumed_input, InputReceiverInterface const* in_input_receiver, INPUT_TYPE in_input, STATE_TYPE const* in_state)
	{
		auto consumed_input_key = std::make_pair(in_input, in_state);

		auto it = inout_consumed_input.find(consumed_input_key);

		// input is not required yet
		if (it == inout_consumed_input.end())
		{
			inout_consumed_input.emplace(consumed_input_key, in_input_receiver);
			return true;
		}
		// the same input receiver requires for the input. that's ok
		if (it->second == in_input_receiver)
			return true;
		// another receiver requires the input. that's a rejection
		return false;
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, Key in_input, InputDeviceInterface const* in_input_device)
	{
		auto const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return false;
		return TryInsertConsumedInput(consumed_keys, in_input_receiver, in_input, state);
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, Input1D in_input, InputDeviceInterface const* in_input_device)
	{
		auto const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return false;
		return TryInsertConsumedInput(consumed_input1D, in_input_receiver, in_input, state);
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, Input2D in_input, InputDeviceInterface const* in_input_device)
	{
		auto const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return false;
		return TryInsertConsumedInput(consumed_input2D, in_input_receiver, in_input, state);
	}

}; // namespace chaos
