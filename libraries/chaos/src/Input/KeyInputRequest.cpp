#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputRequestDebugInfo KeyInputRequest::GetDebugInfo() const
	{
		InputRequestDebugInfo result;		
		result.input = GetKeyName(key);
		return result;
	}

	InputRequestResult KeyInputRequest::Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		// early exit
		if (key == Key::UNKNOWN)
			return InputRequestResult::Invalid;
		// find state
		KeyState const* input_state = in_input_device->GetInputState(key);
		if (input_state == nullptr)
			return InputRequestResult::Invalid; // abnormal (request for an input not handled by the receiver)
		// consum the key of the request (no one can use it anymore until next frame)
		if (!in_consumption_cache.TryConsumeInput(in_input_receiver, key, in_input_device))
			return InputRequestResult::Rejected;
		
		InputStatus input_status = input_state->GetStatus();

		if (HasAnyFlags(action_mask, KeyActionMask::Release))
		{
			if (input_status == InputStatus::BECOME_INACTIVE || input_status == InputStatus::STAY_INACTIVE)
				return InputRequestResult::True;
		}
		if (HasAnyFlags(action_mask, KeyActionMask::Press))
		{
			if (input_status == InputStatus::BECOME_ACTIVE)
				return InputRequestResult::True;
		}
		if (HasAnyFlags(action_mask, KeyActionMask::Repeat))
		{
			if (input_status == InputStatus::STAY_ACTIVE)
				return InputRequestResult::True;
		}
		return InputRequestResult::False;
	}

	bool KeyInputRequest::IsRequestRelatedTo(Key in_input) const
	{
		return (key == in_input);
	}

	KeyInputRequest RequestKeyDown(Key key)
	{
		return KeyInputRequest(key, KeyActionMask::Down);
	}

	KeyInputRequest RequestKeyPressed(Key key)
	{
		return KeyInputRequest(key, KeyActionMask::Press);
	}

	KeyInputRequest RequestKeyRepeat(Key key)
	{
		return KeyInputRequest(key, KeyActionMask::Repeat);
	}

	KeyInputRequest RequestKeyReleased(Key key)
	{
		return KeyInputRequest(key, KeyActionMask::Release);
	}

}; // namespace chaos
