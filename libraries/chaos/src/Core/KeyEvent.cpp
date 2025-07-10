#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool KeyEvent::IsKeyDownEvent() const
	{
		return (action == GLFW_PRESS || action == GLFW_REPEAT);
	}

	bool KeyEvent::IsKeyPressedEvent() const
	{
		return (action == GLFW_PRESS);
	}

	bool KeyEvent::IsKeyRepeatEvent() const
	{
		return (action == GLFW_REPEAT);
	}

	bool KeyEvent::IsKeyReleasedEvent() const
	{
		return (action == GLFW_RELEASE);
	}

	bool KeyEvent::IsKeyEvent(KeyboardButton in_check_button, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (button != in_check_button)
			return false;
		if (in_required_modifiers != KeyModifier::None && !HasAllFlags(key_modifiers, in_required_modifiers))
			return false;
		if (in_forbidden_modifiers != KeyModifier::None && HasAnyFlags(key_modifiers, in_forbidden_modifiers))
			return false;
		return true;
	}

	bool KeyEvent::IsKeyPressed(KeyboardButton in_check_button, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (IsKeyPressedEvent() && IsKeyEvent(in_check_button, in_required_modifiers, in_forbidden_modifiers))
				return true;
		return false;
	}

	bool KeyEvent::IsKeyReleased(KeyboardButton in_check_button, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (IsKeyReleasedEvent() && IsKeyEvent(in_check_button, in_required_modifiers, in_forbidden_modifiers))
			return true;
		return false;
	}

	bool KeyEvent::IsKeyRepeat(KeyboardButton in_check_button, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (IsKeyRepeatEvent() && IsKeyEvent(in_check_button, in_required_modifiers, in_forbidden_modifiers))
			return true;
		return false;
	}

	bool KeyEvent::IsKeyDown(KeyboardButton in_check_button, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (IsKeyDownEvent() && IsKeyEvent(in_check_button, in_required_modifiers, in_forbidden_modifiers))
			return true;
		return false;
	}

}; // namespace chaos
