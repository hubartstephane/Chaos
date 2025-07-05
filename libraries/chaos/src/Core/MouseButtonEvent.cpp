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

	bool MouseButtonEvent::IsButtonEvent(int in_button, int in_check_modifier) const
	{
		if (button == in_button)
			if ((modifier & in_check_modifier) == in_check_modifier)
				return true;
		return false;
	}

	bool MouseButtonEvent::IsButtonPressed(int in_button, int in_check_modifier) const
	{
		if (IsButtonPressedEvent() && IsButtonEvent(in_button, in_check_modifier))
				return true;
		return false;
	}

	bool MouseButtonEvent::IsButtonReleased(int in_button, int in_check_modifier) const
	{
		if (IsButtonReleasedEvent() && IsButtonEvent(in_button, in_check_modifier))
			return true;
		return false;
	}

	bool MouseButtonEvent::IsButtonRepeat(int in_button, int in_check_modifier) const
	{
		if (IsButtonRepeatEvent() && IsButtonEvent(in_button, in_check_modifier))
			return true;
		return false;
	}

	bool MouseButtonEvent::IsButtonDown(int in_button, int in_check_modifier) const
	{
		if (IsButtonDownEvent() && IsButtonEvent(in_button, in_check_modifier))
			return true;
		return false;
	}

}; // namespace chaos
