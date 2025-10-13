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
		consumed_axes.clear();
		consumed_sticks.clear();
		consume_all_inputs = false;
	}

	bool InputConsumptionCache::TryConsumeInput(Key const& in_key, InputDeviceInterface const* in_input_device) // check whether the key is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle key
		result &= DoTryConsumeInput(in_key, in_input_device);
		// handle 'virtual related' inputs
		if (in_key == Key::GAMEPAD_LEFT_TRIGGER)
			result &= DoTryConsumeInput(GamepadAxis::LEFT_TRIGGER, in_input_device);
		if (in_key == Key::GAMEPAD_RIGHT_TRIGGER)
			result &= DoTryConsumeInput(GamepadAxis::RIGHT_TRIGGER, in_input_device);

		return (consume_all_inputs)? 
			false:
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(GamepadAxis in_axis, InputDeviceInterface const* in_input_device) // check whether the axis is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle axis
		result &= DoTryConsumeInput(in_axis, in_input_device);
		// handle 'virtual related' inputs
		if (in_axis == GamepadAxis::LEFT_TRIGGER)
			result &= DoTryConsumeInput(Key::GAMEPAD_LEFT_TRIGGER, in_input_device);
		if (in_axis == GamepadAxis::RIGHT_TRIGGER)
			result &= DoTryConsumeInput(Key::GAMEPAD_RIGHT_TRIGGER, in_input_device);

		if (in_axis == GamepadAxis::LEFT_AXIS_X || in_axis == GamepadAxis::LEFT_AXIS_Y)
			result &= DoTryConsumeInput(GamepadStick::LEFT_STICK, in_input_device);
		if (in_axis == GamepadAxis::RIGHT_AXIS_X || in_axis == GamepadAxis::RIGHT_AXIS_Y)
			result &= DoTryConsumeInput(GamepadStick::RIGHT_STICK, in_input_device);

		return (consume_all_inputs) ?
			false :
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(GamepadStick in_stick, InputDeviceInterface const* in_input_device) // check whether the stick is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle stick
		result &= DoTryConsumeInput(in_stick, in_input_device);
		// handle 'virtual related' inputs
		if (in_stick == GamepadStick::LEFT_STICK)
		{
			result &= DoTryConsumeInput(GamepadAxis::LEFT_AXIS_X, in_input_device);
			result &= DoTryConsumeInput(GamepadAxis::LEFT_AXIS_Y, in_input_device);
		}
		if (in_stick == GamepadStick::RIGHT_STICK)
		{
			result &= DoTryConsumeInput(GamepadAxis::RIGHT_AXIS_X, in_input_device);
			result &= DoTryConsumeInput(GamepadAxis::RIGHT_AXIS_Y, in_input_device);
		}

		return (consume_all_inputs) ?
			false :
			result;
	}

	bool InputConsumptionCache::DoTryConsumeInput(Key in_key, InputDeviceInterface const* in_input_device)
	{
		KeyState const* state = in_input_device->GetInputState(in_key);
		if (state == nullptr)
			return false;
		return consumed_keys.insert(std::make_pair(in_key, state)).second; // insert returns a pair. second element indicates whether the element has effectively been inserted
	}

	bool InputConsumptionCache::DoTryConsumeInput(GamepadAxis in_axis, InputDeviceInterface const* in_input_device)
	{
		AxisState const* state = in_input_device->GetInputState(in_axis);
		if (state == nullptr)
			return false;
		return consumed_axes.insert(std::make_pair(in_axis, state)).second; // insert returns a pair. second element indicates whether the element has effectively been inserted
	}

	bool InputConsumptionCache::DoTryConsumeInput(GamepadStick in_stick, InputDeviceInterface const* in_input_device)
	{
		StickState const* state = in_input_device->GetInputState(in_stick);
		if (state == nullptr)
			return false;
		return consumed_sticks.insert(std::make_pair(in_stick, state)).second; // insert returns a pair. second element indicates whether the element has effectively been inserted
	}

}; // namespace chaos
