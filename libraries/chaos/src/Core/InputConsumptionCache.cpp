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

	void InputConsumptionCache::MarkKeyConsumed(Key in_key, InputDeviceInterface* in_input_device)
	{
		// get state
		KeyState const* state = in_input_device->GetKeyState(in_key);
		if (state == nullptr)
			return;
		// mark the entry as consumed
		auto entry = std::make_pair(in_key, state);
		consumed_keys.insert(entry);
	}

	void InputConsumptionCache::MarkAxisConsumed(GamepadAxis in_axis, InputDeviceInterface* in_input_device)
	{
		// get state
		AxisState const* state = in_input_device->GetAxisState(in_axis);
		if (state == nullptr)
			return;
		// mark the entry as consumed
		auto entry = std::make_pair(in_axis, state);
		consumed_axes.insert(entry);
	}

	void InputConsumptionCache::MarkStickConsumed(GamepadStick in_stick, InputDeviceInterface* in_input_device)
	{
		// get state
		StickState const* state = in_input_device->GetStickState(in_stick);
		if (state == nullptr)
			return;
		// mark the entry as consumed
		auto entry = std::make_pair(in_stick, state);
		consumed_sticks.insert(entry);
	}

	bool InputConsumptionCache::IsKeyMarkedConsumed(Key in_key, InputDeviceInterface const* in_input_device) const
	{
		// get state
		KeyState const* state = in_input_device->GetKeyState(in_key);
		if (state == nullptr)
			return false;
		// search whether the entry is already consumed
		auto entry = std::make_pair(in_key, state);
		return consumed_keys.contains(entry);
	}

	bool InputConsumptionCache::IsAxisMarkedConsumed(GamepadAxis in_axis, InputDeviceInterface const* in_input_device) const
	{
		// get state
		AxisState const* state = in_input_device->GetAxisState(in_axis);
		if (state == nullptr)
			return false;
		// search whether the entry is already consumed
		auto entry = std::make_pair(in_axis, state);
		return consumed_axes.contains(entry);
	}

	bool InputConsumptionCache::IsStickMarkedConsumed(GamepadStick in_stick, InputDeviceInterface const* in_input_device) const
	{
		// get state
		StickState const* state = in_input_device->GetStickState(in_stick);
		if (state == nullptr)
			return false;
		// search whether the entry is already consumed
		auto entry = std::make_pair(in_stick, state);
		return consumed_sticks.contains(entry);
	}

}; // namespace chaos
