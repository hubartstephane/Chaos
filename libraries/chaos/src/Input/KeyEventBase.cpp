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

	bool KeyEventBase::Check(Key in_key, KeyActionMask in_action_mask, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (in_key != key)
			return false;

		if (in_required_modifiers != KeyModifier::None && !HasAllFlags(modifiers, in_required_modifiers))
			return false;
		if (in_forbidden_modifiers != KeyModifier::None && HasAnyFlags(modifiers, in_forbidden_modifiers))
			return false;

		if (HasAnyFlags(in_action_mask, KeyActionMask::Release))
		{
			if (action == KeyAction::Release)
				return true;
		}
		if (HasAnyFlags(in_action_mask, KeyActionMask::Press))
		{
			if (action == KeyAction::Press)
				return true;
		}
		if (HasAnyFlags(in_action_mask, KeyActionMask::Repeat))
		{
			if (action == KeyAction::Repeat)
				return true;
		}
		return false;
	}

	bool KeyEventBase::IsKeyPressed(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		return Check(in_key, KeyActionMask::Press, in_required_modifiers, in_forbidden_modifiers);
	}

	bool KeyEventBase::IsKeyReleased(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		return Check(in_key, KeyActionMask::Release, in_required_modifiers, in_forbidden_modifiers);
	}

	bool KeyEventBase::IsKeyRepeat(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		return Check(in_key, KeyActionMask::Repeat, in_required_modifiers, in_forbidden_modifiers);
	}

	bool KeyEventBase::IsKeyDown(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		return Check(in_key, KeyActionMask::Down, in_required_modifiers, in_forbidden_modifiers);
	}

}; // namespace chaos
