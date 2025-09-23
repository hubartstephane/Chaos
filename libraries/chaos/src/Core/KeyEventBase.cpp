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
		// shuxxx
		return false;




		//return RequestKeyPressed(in_key).RequireModifiers(in_required_modifiers).ForbidModifiers(in_forbidden_modifiers).CheckAgainst(*this);
	}

	bool KeyEventBase::IsKeyReleased(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		// shuxxx
		return false;


		//return RequestKeyReleased(in_key).RequireModifiers(in_required_modifiers).ForbidModifiers(in_forbidden_modifiers).CheckAgainst(*this);
	}

	bool KeyEventBase::IsKeyRepeat(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		// shuxxx
		return false;

		//return RequestKeyRepeat(in_key).RequireModifiers(in_required_modifiers).ForbidModifiers(in_forbidden_modifiers).CheckAgainst(*this);
	}

	bool KeyEventBase::IsKeyDown(Key in_key, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		// shuxxx
		return false;

		//return RequestKeyDown(in_key).RequireModifiers(in_required_modifiers).ForbidModifiers(in_forbidden_modifiers).CheckAgainst(*this);
	}

}; // namespace chaos
