#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool KeyEventBase::IsKeyDownEvent() const
	{
		return (action == KeyAction::Press || action == KeyAction::Repeat);
	}

	bool KeyEventBase::IsKeyPressedEvent() const
	{
		return (action == KeyAction::Press);
	}

	bool KeyEventBase::IsKeyRepeatEvent() const
	{
		return (action == KeyAction::Repeat);
	}

	bool KeyEventBase::IsKeyReleasedEvent() const
	{
		return (action == KeyAction::Release);
	}

	bool KeyEventBase::IsKeyPressed(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		return MatchRequest({in_key, in_required_modifiers, in_forbidden_modifiers, KeyActionMask::Press});
	}

	bool KeyEventBase::IsKeyReleased(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		return MatchRequest({in_key, in_required_modifiers, in_forbidden_modifiers, KeyActionMask::Release});
	}

	bool KeyEventBase::IsKeyRepeat(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		return MatchRequest({in_key, in_required_modifiers, in_forbidden_modifiers, KeyActionMask::Repeat});
	}

	bool KeyEventBase::IsKeyDown(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		return MatchRequest({in_key, in_required_modifiers, in_forbidden_modifiers, KeyActionMask::Down});
	}

	bool KeyEventBase::IsKeyEvent(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (key != in_key)
			return false;
		if (in_required_modifiers != KeyModifier::None && !HasAllFlags(modifiers, in_required_modifiers))
			return false;
		if (in_forbidden_modifiers != KeyModifier::None && HasAnyFlags(modifiers, in_forbidden_modifiers))
			return false;
		return true;
	}

	bool KeyEventBase::MatchRequest(KeyRequest const & in_request) const
	{
		if (!IsKeyEvent(in_request.key, in_request.required_modifiers, in_request.forbidden_modifiers))
			return false;

		switch (action)
		{
		case KeyAction::Release:
			if (!HasAnyFlags(in_request.action_mask, KeyActionMask::Release))
				return false;
			break;

		case KeyAction::Press:
			if (!HasAnyFlags(in_request.action_mask, KeyActionMask::Press))
				return false;
			break;

		case KeyAction::Repeat:
			if (!HasAnyFlags(in_request.action_mask, KeyActionMask::Repeat))
				return false;
			break;

		default:
			assert(0);
		}

		return true;
	}

}; // namespace chaos
