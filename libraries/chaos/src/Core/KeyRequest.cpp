#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool KeyRequest::Check(InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		// early exit
		if (!key.IsValid())
			return false;
		// consum the key of the request (no one can use it anymore until next frame)
		if (!in_consumption_cache.TryConsumeInput(key, in_input_device))
			return false;

		// find key
		KeyState const* key_state = in_input_device->GetKeyState(key);
		if (key_state == nullptr)
			return false;


		KeyStatus key_status = key_state->GetStatus();

		if (key == KeyboardButton::LEFT)
		{
			if (key_status == KeyStatus::BECOME_PRESSED)
				in_input_device = in_input_device;
			if (key_status == KeyStatus::STAY_PRESSED)
				in_input_device = in_input_device;
			if (key_status == KeyStatus::STAY_RELEASED)
				in_input_device = in_input_device;
			if (key_status == KeyStatus::BECOME_RELEASED)
				in_input_device = in_input_device;



		}







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
					return false;
			if (HasAnyFlags(forbidden_modifiers, KeyModifier::Alt))
				if (alt_value)
					return false;

			bool shift_value = GetModifierKeyValue(KeyboardButton::LEFT_SHIFT, KeyboardButton::RIGHT_SHIFT);
			if (HasAnyFlags(required_modifiers, KeyModifier::Shift))
				if (!shift_value)
					return false;
			if (HasAnyFlags(forbidden_modifiers, KeyModifier::Shift))
				if (shift_value)
					return false;

			bool control_value = GetModifierKeyValue(KeyboardButton::LEFT_CONTROL, KeyboardButton::RIGHT_CONTROL);
			if (HasAnyFlags(required_modifiers, KeyModifier::Control))
				if (!control_value)
					return false;
			if (HasAnyFlags(forbidden_modifiers, KeyModifier::Control))
				if (control_value)
					return false;
		}

		if (HasAnyFlags(action_mask, KeyActionMask::Release))
		{
			if (key_status == KeyStatus::BECOME_RELEASED || key_status == KeyStatus::STAY_RELEASED)
				return true;
		}
		if (HasAnyFlags(action_mask, KeyActionMask::Press))
		{
			if (key_status == KeyStatus::BECOME_PRESSED)
				return true;
		}
		if (HasAnyFlags(action_mask, KeyActionMask::Repeat))
		{
			if (key_status == KeyStatus::STAY_PRESSED)
				return true;
		}
		return false;
	}

	bool KeyRequest::Check(KeyEventBase const& in_key_event, InputDeviceInterface const * in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		// early exit
		if (!key.IsValid())
			return false;
		// consum the key of the request (no one can use it anymore until next frame)
		if (!in_consumption_cache.TryConsumeInput(key, in_input_device))
			return false;
		//  effective request
		if (key != in_key_event.key)
			return false;

		if (required_modifiers != KeyModifier::None && !HasAllFlags(in_key_event.modifiers, required_modifiers))
			return false;
		if (forbidden_modifiers != KeyModifier::None && HasAnyFlags(in_key_event.modifiers, forbidden_modifiers))
			return false;

		if (HasAnyFlags(action_mask, KeyActionMask::Release))
		{
			if (in_key_event.action == KeyAction::Release)
				return true;
		}
		if (HasAnyFlags(action_mask, KeyActionMask::Press))
		{
			if (in_key_event.action == KeyAction::Press)
				return true;
		}
		if (HasAnyFlags(action_mask, KeyActionMask::Repeat))
		{
			if (in_key_event.action == KeyAction::Repeat)
				return true;
		}
		return false;
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
		return KeyRequest(key,KeyActionMask::Press);
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
