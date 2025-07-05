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

	bool KeyEvent::IsKeyEvent(KeyboardButton in_check_button, int in_check_modifier) const
	{
		if (button == in_check_button)
			if ((modifier & in_check_modifier) == in_check_modifier)
				return true;
		return false;
	}

	bool KeyEvent::IsKeyPressed(KeyboardButton in_check_button, int in_check_modifier) const
	{
		if (IsKeyPressedEvent() && IsKeyEvent(in_check_button, in_check_modifier))
				return true;
		return false;
	}

	bool KeyEvent::IsKeyReleased(KeyboardButton in_check_button, int in_check_modifier) const
	{
		if (IsKeyReleasedEvent() && IsKeyEvent(in_check_button, in_check_modifier))
			return true;
		return false;
	}

	bool KeyEvent::IsKeyRepeat(KeyboardButton in_check_button, int in_check_modifier) const
	{
		if (IsKeyRepeatEvent() && IsKeyEvent(in_check_button, in_check_modifier))
			return true;
		return false;
	}

	bool KeyEvent::IsKeyDown(KeyboardButton in_check_button, int in_check_modifier) const
	{
		if (IsKeyDownEvent() && IsKeyEvent(in_check_button, in_check_modifier))
			return true;
		return false;
	}

}; // namespace chaos
