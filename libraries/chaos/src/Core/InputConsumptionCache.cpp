#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void InputConsumptionCache::Clear()
	{
		consumed_keys.clear();
		consumed_axes.clear();
		consumed_sticks.clear();
	}

	bool InputConsumptionCache::CheckAndMarkKeyConsumed(Key in_key, InputDeviceInterface const* in_input_device)
	{
		KeyState const* state = in_input_device->GetKeyState(in_key);
		return CheckAndMarkKeyConsumed(in_key, state);
	}

	bool InputConsumptionCache::CheckAndMarkAxisConsumed(GamepadAxis in_axis, InputDeviceInterface const* in_input_device)
	{
		AxisState const* state = in_input_device->GetAxisState(in_axis);
		return CheckAndMarkAxisConsumed(in_axis, state);
	}

	bool InputConsumptionCache::CheckAndMarkStickConsumed(GamepadStick in_stick, InputDeviceInterface const* in_input_device)
	{
		StickState const* state = in_input_device->GetStickState(in_stick);
		return CheckAndMarkStickConsumed(in_stick, state);
	}

	bool InputConsumptionCache::CheckAndMarkKeyConsumed(Key in_key, KeyState const * in_state)
	{
		if (in_state == nullptr)
			return false;
		return consumed_keys.insert(std::make_pair(in_key, in_state)).second; // insert returns a pair. second element indicates whether the element has effectively been inserted
	}

	bool InputConsumptionCache::CheckAndMarkAxisConsumed(GamepadAxis in_axis, AxisState const* in_state)
	{
		if (in_state == nullptr)
			return false;
		return consumed_axes.insert(std::make_pair(in_axis, in_state)).second; // insert returns a pair. second element indicates whether the element has effectively been inserted
	}

	bool InputConsumptionCache::CheckAndMarkStickConsumed(GamepadStick in_stick, StickState const* in_state)
	{
		if (in_state == nullptr)
			return false;
		return consumed_sticks.insert(std::make_pair(in_stick, in_state)).second; // insert returns a pair. second element indicates whether the element has effectively been inserted
	}

}; // namespace chaos
