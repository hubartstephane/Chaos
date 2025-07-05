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

	bool KeyEvent::IsKeyEvent(KeyboardButton check_button, int check_modifier) const
	{
		if (button == check_button)
			if ((modifier & check_modifier) == check_modifier)
				return true;
		return false;
	}

	bool KeyEvent::IsKeyPressed(KeyboardButton check_button, int check_modifier) const
	{
		if (IsKeyPressedEvent() && IsKeyEvent(check_button, check_modifier))
				return true;
		return false;
	}

	bool KeyEvent::IsKeyReleased(KeyboardButton check_button, int check_modifier) const
	{
		if (IsKeyReleasedEvent() && IsKeyEvent(check_button, check_modifier))
			return true;
		return false;
	}

	bool KeyEvent::IsKeyRepeat(KeyboardButton check_button, int check_modifier) const
	{
		if (IsKeyRepeatEvent() && IsKeyEvent(check_button, check_modifier))
			return true;
		return false;
	}

	bool KeyEvent::IsKeyDown(KeyboardButton check_button, int check_modifier) const
	{
		if (IsKeyDownEvent() && IsKeyEvent(check_button, check_modifier))
			return true;
		return false;
	}

}; // namespace chaos
