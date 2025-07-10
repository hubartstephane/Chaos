#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool MouseButtonEvent::IsButtonDownEvent() const
	{
		return (action == GLFW_PRESS || action == GLFW_REPEAT);
	}

	bool MouseButtonEvent::IsButtonPressedEvent() const
	{
		return (action == GLFW_PRESS);
	}

	bool MouseButtonEvent::IsButtonRepeatEvent() const
	{
		return (action == GLFW_REPEAT);
	}

	bool MouseButtonEvent::IsButtonReleasedEvent() const
	{
		return (action == GLFW_RELEASE);
	}

	bool MouseButtonEvent::IsButtonEvent(int in_button, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (button != in_button)
			return false;
		if (in_required_modifiers != KeyModifier::None && !HasAllFlags(key_modifiers, in_required_modifiers))
			return false;
		if (in_forbidden_modifiers != KeyModifier::None && HasAnyFlags(key_modifiers, in_forbidden_modifiers))
			return false;
		return true;
	}

	bool MouseButtonEvent::IsButtonPressed(int in_button, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (IsButtonPressedEvent() && IsButtonEvent(in_button, in_required_modifiers, in_forbidden_modifiers))
				return true;
		return false;
	}

	bool MouseButtonEvent::IsButtonReleased(int in_button, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (IsButtonReleasedEvent() && IsButtonEvent(in_button, in_required_modifiers, in_forbidden_modifiers))
			return true;
		return false;
	}

	bool MouseButtonEvent::IsButtonRepeat(int in_button, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (IsButtonRepeatEvent() && IsButtonEvent(in_button, in_required_modifiers, in_forbidden_modifiers))
			return true;
		return false;
	}

	bool MouseButtonEvent::IsButtonDown(int in_button, KeyModifier in_required_modifiers, KeyModifier in_forbidden_modifiers) const
	{
		if (IsButtonDownEvent() && IsButtonEvent(in_button, in_required_modifiers, in_forbidden_modifiers))
			return true;
		return false;
	}

}; // namespace chaos
