#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<KeyStatusRequestType> const KeyStatusRequestType_metadata =
	{
		{ KeyStatusRequestType::Up, "Up" },
		{ KeyStatusRequestType::Release, "Release" },
		{ KeyStatusRequestType::Press, "Press" },
		{ KeyStatusRequestType::Repeat, "Repeat" },
		{ KeyStatusRequestType::Down, "Down" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(KeyStatusRequestType, &KeyStatusRequestType_metadata, CHAOS_API);

	char const * KeyInputRequest::GetDebugInfo(InputRequestDebugInfoStorage & debug_info_storage) const
	{
		std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "%s [%s]", EnumToString(request_type), GetKeyName(key));
		return debug_info_storage.buffer;
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
			
		auto ConvertResultType = [](bool result)
		{
			return result? InputRequestResult::True: InputRequestResult::False;
		};

		InputStatus input_status = input_state->GetStatus();
		switch (request_type)
		{
		case KeyStatusRequestType::Up:
			return ConvertResultType(input_status == InputStatus::BECOME_INACTIVE || input_status == InputStatus::STAY_INACTIVE);
		case KeyStatusRequestType::Release:
			return ConvertResultType(input_status == InputStatus::BECOME_INACTIVE);
		case KeyStatusRequestType::Press:
			return ConvertResultType(input_status == InputStatus::BECOME_ACTIVE);
		case KeyStatusRequestType::Repeat:
			return ConvertResultType(input_status == InputStatus::STAY_ACTIVE);
		case KeyStatusRequestType::Down:
			return ConvertResultType(input_status == InputStatus::BECOME_ACTIVE || input_status == InputStatus::STAY_ACTIVE);
		default:

			assert(0);
		}
		return InputRequestResult::False;
	}

	bool KeyInputRequest::IsRequestRelatedTo(Key in_input) const
	{
		return (key == in_input);
	}

	KeyInputRequest KeyUp(Key key)
	{
		return KeyInputRequest(key, KeyStatusRequestType::Up);
	}

	KeyInputRequest KeyReleased(Key key)
	{
		return KeyInputRequest(key, KeyStatusRequestType::Release);
	}

	KeyInputRequest KeyPressed(Key key)
	{
		return KeyInputRequest(key, KeyStatusRequestType::Press);
	}

	KeyInputRequest KeyRepeat(Key key)
	{
		return KeyInputRequest(key, KeyStatusRequestType::Repeat);
	}

	KeyInputRequest KeyDown(Key key)
	{
		return KeyInputRequest(key, KeyStatusRequestType::Down);
	}

}; // namespace chaos
