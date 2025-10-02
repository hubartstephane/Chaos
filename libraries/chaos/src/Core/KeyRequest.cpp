#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	std::string KeyRequest::GetInputTitle() const
	{
		return key.GetName();
	}

	InputRequestResult KeyRequest::Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		// early exit
		if (!key.IsValid())
			return InputRequestResult::Invalid;
		// find state
		KeyState const* input_state = in_input_device->GetInputState(key);
		if (input_state == nullptr)
			return InputRequestResult::Invalid; // abnormal (request for an input not handled by the receiver)
		// consum the key of the request (no one can use it anymore until next frame)
		if (!in_consumption_cache.TryConsumeInput(key, in_input_device))
			return InputRequestResult::Rejected;
		
		if (required_modifiers != KeyModifier::None || forbidden_modifiers != KeyModifier::None)
		{
			auto GetModifierKeyValue = [in_input_device](KeyboardButton key1, KeyboardButton key2)
			{
				KeyStatus key1_status = in_input_device->GetKeyStatus(key1);
				if (key1_status == KeyStatus::BECOME_PRESSED || key1_status == KeyStatus::STAY_PRESSED)
					return true;
				KeyStatus key2_status = in_input_device->GetKeyStatus(key2);
				if (key2_status == KeyStatus::BECOME_PRESSED || key2_status == KeyStatus::STAY_PRESSED)
					return true;
				return false;
			};

			bool alt_value = GetModifierKeyValue(KeyboardButton::LEFT_ALT, KeyboardButton::RIGHT_ALT);
			if (HasAnyFlags(required_modifiers, KeyModifier::Alt))
				if (!alt_value)
					return InputRequestResult::False;
			if (HasAnyFlags(forbidden_modifiers, KeyModifier::Alt))
				if (alt_value)
					return InputRequestResult::False;

			bool shift_value = GetModifierKeyValue(KeyboardButton::LEFT_SHIFT, KeyboardButton::RIGHT_SHIFT);
			if (HasAnyFlags(required_modifiers, KeyModifier::Shift))
				if (!shift_value)
					return InputRequestResult::False;
			if (HasAnyFlags(forbidden_modifiers, KeyModifier::Shift))
				if (shift_value)
					return InputRequestResult::False;

			bool control_value = GetModifierKeyValue(KeyboardButton::LEFT_CONTROL, KeyboardButton::RIGHT_CONTROL);
			if (HasAnyFlags(required_modifiers, KeyModifier::Control))
				if (!control_value)
					return InputRequestResult::False;
			if (HasAnyFlags(forbidden_modifiers, KeyModifier::Control))
				if (control_value)
					return InputRequestResult::False;
		}

		KeyStatus input_status = input_state->GetStatus();

		if (HasAnyFlags(action_mask, KeyActionMask::Release))
		{
			if (input_status == KeyStatus::BECOME_RELEASED || input_status == KeyStatus::STAY_RELEASED)
				return InputRequestResult::True;
		}
		if (HasAnyFlags(action_mask, KeyActionMask::Press))
		{
			if (input_status == KeyStatus::BECOME_PRESSED)
				return InputRequestResult::True;
		}
		if (HasAnyFlags(action_mask, KeyActionMask::Repeat))
		{
			if (input_status == KeyStatus::STAY_PRESSED)
				return InputRequestResult::True;
		}
		return InputRequestResult::False;
	}

	InputRequestResult KeyRequest::Check(InputReceiverInterface const* in_input_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const * in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		// early exit
		if (!key.IsValid())
			return InputRequestResult::Invalid;
		// find state
		KeyState const* input_state = in_input_device->GetInputState(key);
		if (input_state == nullptr)
			return InputRequestResult::Invalid; // abnormal (request for an input not handled by the receiver)
		// consum the key of the request (no one can use it anymore until next frame)
		if (!in_consumption_cache.TryConsumeInput(key, in_input_device))
			return InputRequestResult::Rejected;
		//  effective request
		return in_key_event.Check(key, action_mask, required_modifiers, forbidden_modifiers)?
			InputRequestResult::True:
			InputRequestResult::False;
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
