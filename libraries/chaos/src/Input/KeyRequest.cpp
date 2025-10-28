#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputRequestDebugInfo KeyRequest::GetDebugInfo() const
	{
		char buffer[256];

		InputRequestDebugInfo result;		
		result.input = GetKeyName(key);
		if (required_modifiers != KeyModifier::None)
			result.required_modifiers = EnumToString(required_modifiers, buffer, 256);
		if (forbidden_modifiers != KeyModifier::None)
			result.forbidden_modifiers = EnumToString(forbidden_modifiers, buffer, 256);
		result.action_type = EnumToString(action_mask, buffer, 256);
		return result;
	}

	InputRequestResult KeyRequest::Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const
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
		
		if (required_modifiers != KeyModifier::None || forbidden_modifiers != KeyModifier::None)
		{
			auto GetModifierKeyValue = [in_input_device](Key key1, Key key2)
			{
				InputStatus key1_status = in_input_device->GetInputStatus(key1);
				if (key1_status == InputStatus::BECOME_ACTIVE || key1_status == InputStatus::STAY_ACTIVE)
					return true;
				InputStatus key2_status = in_input_device->GetInputStatus(key2);
				if (key2_status == InputStatus::BECOME_ACTIVE || key2_status == InputStatus::STAY_ACTIVE)
					return true;
				return false;
			};

			bool alt_value = GetModifierKeyValue(Key::LEFT_ALT, Key::RIGHT_ALT);
			if (HasAnyFlags(required_modifiers, KeyModifier::Alt))
				if (!alt_value)
					return InputRequestResult::False;
			if (HasAnyFlags(forbidden_modifiers, KeyModifier::Alt))
				if (alt_value)
					return InputRequestResult::False;

			bool shift_value = GetModifierKeyValue(Key::LEFT_SHIFT, Key::RIGHT_SHIFT);
			if (HasAnyFlags(required_modifiers, KeyModifier::Shift))
				if (!shift_value)
					return InputRequestResult::False;
			if (HasAnyFlags(forbidden_modifiers, KeyModifier::Shift))
				if (shift_value)
					return InputRequestResult::False;

			bool control_value = GetModifierKeyValue(Key::LEFT_CONTROL, Key::RIGHT_CONTROL);
			if (HasAnyFlags(required_modifiers, KeyModifier::Control))
				if (!control_value)
					return InputRequestResult::False;
			if (HasAnyFlags(forbidden_modifiers, KeyModifier::Control))
				if (control_value)
					return InputRequestResult::False;
		}

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

	bool KeyRequest::IsRequestRelatedTo(Key in_input) const
	{
		return (key == in_input);
	}

	KeyRequest KeyRequest::RequireModifiers(KeyModifier in_modifiers) const
	{
		KeyRequest result = KeyRequest(key, action_mask);
		result.required_modifiers = in_modifiers;
		result.forbidden_modifiers = forbidden_modifiers;
		return result;
	}

	KeyRequest KeyRequest::ForbidModifiers(KeyModifier in_modifiers) const
	{
		KeyRequest result = KeyRequest(key, action_mask);
		result.required_modifiers = required_modifiers;
		result.forbidden_modifiers = in_modifiers;
		return result;
	}

	KeyRequest RequestKeyDown(Key key)
	{
		return KeyRequest(key, KeyActionMask::Down);
	}

	KeyRequest RequestKeyPressed(Key key)
	{
		return KeyRequest(key, KeyActionMask::Press);
	}

	KeyRequest RequestKeyRepeat(Key key)
	{
		return KeyRequest(key, KeyActionMask::Repeat);
	}

	KeyRequest RequestKeyReleased(Key key)
	{
		return KeyRequest(key, KeyActionMask::Release);
	}

}; // namespace chaos
