
#include <chaos/InputEventReceiver.h>

namespace chaos
{
	void InputEventReceiver::SetInputMode(InputMode new_mode)
	{
		if (new_mode == input_mode)
			return;
		InputMode old_mode = input_mode;
		input_mode = new_mode;
		OnInputModeChanged(new_mode, old_mode);
	}

	void InputEventReceiver::OnInputModeChanged(InputMode new_mode, InputMode old_mode) 
	{
	}

	bool InputEventReceiver::OnMouseMove(double x, double y) 
	{
		if (OnMouseMoveImpl(x, y))
		{
			SetInputMode(InputMode::Mouse);
			return true;
		}
		return false; 
	}

	bool InputEventReceiver::OnMouseButton(int button, int action, int modifier) 
	{ 
		if (OnMouseButtonImpl(button, action, modifier))
		{
			SetInputMode(InputMode::Mouse);
			return true;
		}
		return false; 
	}

	bool InputEventReceiver::OnMouseWheel(double scroll_x, double scroll_y) 
	{ 
		if (OnMouseWheelImpl(scroll_x, scroll_y))
		{
			SetInputMode(InputMode::Mouse);
			return true;
		}
		return false; 
	}

	bool InputEventReceiver::OnKeyEvent(int key, int scan_code, int action, int modifier) 
	{ 
		if (OnKeyEventImpl(key, scan_code, action, modifier))
		{
			SetInputMode(InputMode::Keyboard);
			return true;
		}
		return false; 
	}

	bool InputEventReceiver::OnCharEvent(unsigned int c) 
	{ 
		if (OnCharEventImpl(c))
		{
			SetInputMode(InputMode::Keyboard);
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

	bool InputEventReceiver::OnKeyEventImpl(int key, int scan_code, int action, int modifier)
	{
		return false;
	}

	bool InputEventReceiver::OnCharEventImpl(unsigned int c)
	{
		return false;
	}

}; // namespace chaos
