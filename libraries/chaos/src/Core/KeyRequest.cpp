#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool KeyRequest::CheckAgainst(InputDeviceInterface const * in_input_device) const
	{
		KeyStatus key_status = in_input_device->GetKeyStatus(key);

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

	bool KeyRequest::CheckAgainst(KeyEventBase const & key_event) const
	{
		if (key != key_event.key)
			return false;

		if (required_modifiers != KeyModifier::None && !HasAllFlags(key_event.modifiers, required_modifiers))
			return false;
		if (forbidden_modifiers != KeyModifier::None && HasAnyFlags(key_event.modifiers, forbidden_modifiers))
			return false;

		if (HasAnyFlags(action_mask, KeyActionMask::Release))
		{
			if (key_event.action == KeyAction::Release)
				return true;
		}
		if (HasAnyFlags(action_mask, KeyActionMask::Press))
		{
			if (key_event.action == KeyAction::Press)
				return true;
		}
		if (HasAnyFlags(action_mask, KeyActionMask::Repeat))
		{
			if (key_event.action == KeyAction::Repeat)
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
