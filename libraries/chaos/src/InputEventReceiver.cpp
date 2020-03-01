
#include <chaos/InputEventReceiver.h>

namespace chaos
{
	bool KeyEvent::IsKeyPressed() const
	{
		return (action == GLFW_PRESS || action == GLFW_REPEAT);
	}

	bool KeyEvent::IsKeyReleased() const
	{
		return (action == GLFW_RELEASE);
	}

	bool KeyEvent::IsKeyEvent(int check_key, int check_modifier) const
	{
		if (key == check_key)
			if ((modifier & check_modifier) == check_modifier)
				return true;
		return false;
	}

	bool KeyEvent::IsKeyPressed(int check_key, int check_modifier) const
	{
		if (IsKeyPressed() && IsKeyEvent(check_key, check_modifier))
				return true;
		return false;
	}

	bool KeyEvent::IsKeyReleased(int check_key, int check_modifier) const
	{
		if (IsKeyReleased() && IsKeyEvent(check_key, check_modifier))
			return true;
		return false;
	}

	void InputEventReceiver::SetInputMode(InputMode new_mode)
	{
		if (new_mode == input_mode)
			return;
		InputMode old_mode = input_mode;
		input_mode = new_mode;
		OnInputModeChanged(new_mode, old_mode);
	}

	bool InputEventReceiver::CheckKeyPressed(int check_key)
	{
		GLFWwindow* glfw_window = GetGLFWWindow();
		if (glfw_window == nullptr)
			return false;
		if (glfwGetKey(glfw_window, check_key))
		{
			SetInputMode(InputMode::KEYBOARD);
			return true;
		}
		return false;
	}

	bool InputEventReceiver::CheckKeyPressed(KeyEvent const& event, int check_key, int check_modifier)
	{
		if (event.IsKeyPressed(check_key, check_modifier))
		{
			SetInputMode(InputMode::KEYBOARD);
			return true;
		}
		return false;
	}

	void InputEventReceiver::OnInputModeChanged(InputMode new_mode, InputMode old_mode) 
	{
	}

	bool InputEventReceiver::OnMouseMove(double x, double y) 
	{
		if (OnMouseMoveImpl(x, y))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false; 
	}

	bool InputEventReceiver::OnMouseButton(int button, int action, int modifier) 
	{ 
		if (OnMouseButtonImpl(button, action, modifier))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false; 
	}

	bool InputEventReceiver::OnMouseWheel(double scroll_x, double scroll_y) 
	{ 
		if (OnMouseWheelImpl(scroll_x, scroll_y))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false; 
	}

	bool InputEventReceiver::OnKeyEvent(KeyEvent const & event) 
	{ 
		if (OnKeyEventImpl(event))
		{
			SetInputMode(InputMode::KEYBOARD);
			return true;
		}
		return false; 
	}

	bool InputEventReceiver::OnCharEvent(unsigned int c) 
	{ 
		if (OnCharEventImpl(c))
		{
			SetInputMode(InputMode::KEYBOARD);
			return true;
		}
		return false; 
	}

	bool InputEventReceiver::OnMouseMoveImpl(double x, double y)
	{
		return false;
	}

	bool InputEventReceiver::OnMouseButtonImpl(int button, int action, int modifier)
	{
		return false;
	}

	bool InputEventReceiver::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		return false;
	}

	bool InputEventReceiver::OnKeyEventImpl(KeyEvent const & event)
	{
		return false;
	}

	bool InputEventReceiver::OnCharEventImpl(unsigned int c)
	{
		return false;
	}

}; // namespace chaos
