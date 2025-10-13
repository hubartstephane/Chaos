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
			result &= DoTryConsumeInput(Input1D::LEFT_TRIGGER, in_input_device);
		if (in_key == Key::GAMEPAD_RIGHT_TRIGGER)
			result &= DoTryConsumeInput(Input1D::RIGHT_TRIGGER, in_input_device);

		return (consume_all_inputs)? 
			false:
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(Input1D in_input, InputDeviceInterface const* in_input_device) // check whether the axis is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle axis
		result &= DoTryConsumeInput(in_input, in_input_device);
		// handle 'virtual related' inputs
		if (in_input == Input1D::LEFT_TRIGGER)
			result &= DoTryConsumeInput(Key::GAMEPAD_LEFT_TRIGGER, in_input_device);
		if (in_input == Input1D::RIGHT_TRIGGER)
			result &= DoTryConsumeInput(Key::GAMEPAD_RIGHT_TRIGGER, in_input_device);

		if (in_input == Input1D::LEFT_AXIS_X || in_input == Input1D::LEFT_AXIS_Y)
			result &= DoTryConsumeInput(Input2D::LEFT_STICK, in_input_device);
		if (in_input == Input1D::RIGHT_AXIS_X || in_input == Input1D::RIGHT_AXIS_Y)
			result &= DoTryConsumeInput(Input2D::RIGHT_STICK, in_input_device);

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
		if (in_input == Input2D::LEFT_STICK)
		{
			result &= DoTryConsumeInput(Input1D::LEFT_AXIS_X, in_input_device);
			result &= DoTryConsumeInput(Input1D::LEFT_AXIS_Y, in_input_device);
		}
		if (in_input == Input2D::RIGHT_STICK)
		{
			result &= DoTryConsumeInput(Input1D::RIGHT_AXIS_X, in_input_device);
			result &= DoTryConsumeInput(Input1D::RIGHT_AXIS_Y, in_input_device);
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

	bool InputConsumptionCache::DoTryConsumeInput(Input1D in_input, InputDeviceInterface const* in_input_device)
	{
		Input1DState const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return false;
		return consumed_axes.insert(std::make_pair(in_input, state)).second; // insert returns a pair. second element indicates whether the element has effectively been inserted
	}

	bool InputConsumptionCache::DoTryConsumeInput(Input2D in_input, InputDeviceInterface const* in_input_device)
	{
		Input2DState const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return false;
		return consumed_sticks.insert(std::make_pair(in_input, state)).second; // insert returns a pair. second element indicates whether the element has effectively been inserted
	}

}; // namespace chaos
