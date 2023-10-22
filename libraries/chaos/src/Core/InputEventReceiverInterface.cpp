#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	//
	// KeyEvent functions
	//

	bool KeyEvent::IsKeyPressed() const
	{
		return (action == GLFW_PRESS || action == GLFW_REPEAT);
	}

	bool KeyEvent::IsKeyReleased() const
	{
		return (action == GLFW_RELEASE);
	}

	bool KeyEvent::IsKeyEvent(int check_keycode, int check_modifier) const
	{
		if (keycode == check_keycode)
			if ((modifier & check_modifier) == check_modifier)
				return true;
		return false;
	}

	bool KeyEvent::IsKeyPressed(int check_keycode, int check_modifier) const
	{
		if (IsKeyPressed() && IsKeyEvent(check_keycode, check_modifier))
				return true;
		return false;
	}

	bool KeyEvent::IsKeyReleased(int check_keycode, int check_modifier) const
	{
		if (IsKeyReleased() && IsKeyEvent(check_keycode, check_modifier))
			return true;
		return false;
	}

	//
	// InputEventReceiverInterface functions
	//

	void InputEventReceiverInterface::SetInputMode(InputMode new_mode)
	{
		if (new_mode == input_mode)
			return;
		InputMode old_mode = input_mode;
		input_mode = new_mode;
		OnInputModeChanged(new_mode, old_mode);
	}

	bool InputEventReceiverInterface::CheckKeyPressed(Key const* keys, bool previous_frame)
	{
		// early exit
		if (keys == nullptr)
			return false;
		// iteration
		for (size_t i = 0; keys[i].IsValid(); ++i)
			if (CheckKeyPressed(keys[i], previous_frame))
				return true;
		return false;
	}

	bool InputEventReceiverInterface::CheckKeyPressed(Key key, bool previous_frame)
	{
		if (DoCheckKeyPressed(key, previous_frame))
		{
			if (key.GetType() == KeyType::KEYBOARD)
				SetInputMode(InputMode::KEYBOARD);
			else if (key.GetType() == KeyType::MOUSE)
				SetInputMode(InputMode::MOUSE);
			else if (key.GetType() == KeyType::GAMEPAD)
				SetInputMode(InputMode::GAMEPAD);
			return true;
		}
		return false;
	}

	bool InputEventReceiverInterface::DoCheckKeyPressed(Key key, bool previous_frame)
	{
		if (ButtonState const* state = KeyboardState::GetKeyState(key))
			return state->IsPressed(previous_frame);
		return false;
	}

	void InputEventReceiverInterface::OnInputModeChanged(InputMode new_mode, InputMode old_mode) 
	{
	}

	bool InputEventReceiverInterface::OnMouseMove(glm::vec2 const& delta)
	{
		if (OnMouseMoveImpl(delta))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false; 
	}

	bool InputEventReceiverInterface::OnMouseButton(int button, int action, int modifier) 
	{ 
		if (OnMouseButtonImpl(button, action, modifier))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false; 
	}

	bool InputEventReceiverInterface::OnMouseWheel(double scroll_x, double scroll_y) 
	{ 
		if (OnMouseWheelImpl(scroll_x, scroll_y))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false; 
	}

	bool InputEventReceiverInterface::OnKeyEvent(KeyEvent const & event) 
	{ 
		if (OnKeyEventImpl(event))
		{
			SetInputMode(InputMode::KEYBOARD);
			return true;
		}
		return false; 
	}

	bool InputEventReceiverInterface::OnCharEvent(unsigned int c) 
	{ 
		if (OnCharEventImpl(c))
		{
			SetInputMode(InputMode::KEYBOARD);
			return true;
		}
		return false; 
	}

	bool InputEventReceiverInterface::OnMouseMoveImpl(glm::vec2 const& delta)
	{
		return false;
	}

	bool InputEventReceiverInterface::OnMouseButtonImpl(int button, int action, int modifier)
	{
		return false;
	}

	bool InputEventReceiverInterface::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		return false;
	}

	bool InputEventReceiverInterface::OnKeyEventImpl(KeyEvent const & event)
	{
		return false;
	}

	bool InputEventReceiverInterface::OnCharEventImpl(unsigned int c)
	{
		return false;
	}

}; // namespace chaos
