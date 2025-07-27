#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool KeyRequest::MatchEvent(KeyEventBase const & key_event) const
	{
		if (key != key_event.key)
			return false;
		if (required_modifiers != KeyModifier::None && !HasAllFlags(key_event.modifiers, required_modifiers))
			return false;
		if (forbidden_modifiers != KeyModifier::None && HasAnyFlags(key_event.modifiers, forbidden_modifiers))
			return false;

		switch (key_event.action)
		{
		case KeyAction::Release:
			if (!HasAnyFlags(action_mask, KeyActionMask::Release))
				return false;
			break;

		case KeyAction::Press:
			if (!HasAnyFlags(action_mask, KeyActionMask::Press))
				return false;
			break;

		case KeyAction::Repeat:
			if (!HasAnyFlags(action_mask, KeyActionMask::Repeat))
				return false;
			break;

		default:
			assert(0);
		}
		return true;
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
