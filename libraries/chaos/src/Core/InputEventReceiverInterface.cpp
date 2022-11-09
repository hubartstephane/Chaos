#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	//
	// ButtonState functions
	//

	void ButtonState::SetValue(bool in_value)
	{
		previous_value = value;
		value = in_value;
	}

	void ButtonState::UpdateSameValueTimer(float delta_time)
	{
		if (value != previous_value)
			same_value_timer = 0.0f;
		else
			same_value_timer += delta_time;
	}

	
	bool ButtonState::IsPressed(bool previous_frame) const
	{
		return GetValue(previous_frame);
	}
	
	bool ButtonState::IsJustPressed() const
	{
		return IsPressed(false) && !IsPressed(true);
	}

	bool ButtonState::IsJustReleased() const
	{
		return !IsPressed(false) && IsPressed(true);
	}

	ButtonStateChange ButtonState::GetStateChange() const
	{
		bool current_state = IsPressed(false);
		bool previous_state = IsPressed(true);

		if (current_state == previous_state)
			return (current_state) ? ButtonStateChange::STAY_PRESSED : ButtonStateChange::STAY_RELEASED;
		else
			return (current_state) ? ButtonStateChange::BECOME_PRESSED : ButtonStateChange::BECOME_RELEASED;
	}

	//
	// AxisState functions
	//

	// XXX : some sticks are not abled to physicaly returns 1.0 when they are fully triggered (depend on the device)
	//       that's why i use some min/max values (initialized with a coherent value)
	//       if the stick goes further than theses values, we update them.
	//       that help us to have a good evaluation of the stick range over time.

	void AxisState::SetValue(float in_raw_value, float dead_zone)
	{
		previous_value = value;

		// apply the dead zone
		if (in_raw_value < dead_zone && in_raw_value > -dead_zone)
			in_raw_value = 0.0f;

		// clamp the raw value to -1 .. +1
		in_raw_value = std::clamp(in_raw_value, -1.0f, +1.0f);

		// store raw value
		max_value = std::max(max_value, in_raw_value);
		min_value = std::min(min_value, in_raw_value);

		// apply dead zone and normalization
		value = 0.0f;
		if (in_raw_value > dead_zone || in_raw_value < -dead_zone)
		{
			if (in_raw_value > 0.0f)
				value = (in_raw_value - dead_zone) / (max_value - dead_zone);
			else
				value = -(in_raw_value + dead_zone) / (min_value + dead_zone);
		}
	}

	void AxisState::UpdateSameValueTimer(float delta_time)
	{
		// update timer
		bool reset_timer =
			(value == 0.0f && previous_value != 0.0f) ||
			(value != 0.0f && previous_value == 0.0f) ||
			(value * previous_value < 0.0f);
		if (reset_timer)
			same_value_timer = 0.0f;
		else
			same_value_timer += delta_time;
	}

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

	bool InputEventReceiverInterface::CheckButtonPressed(Key const* buttons, bool previous_frame)
	{
		// early exit
		if (buttons == nullptr)
			return false;
		// iteration
		for (size_t i = 0; buttons[i].IsValid(); ++i)
			if (CheckButtonPressed(buttons[i], previous_frame))
				return true;
		return false;
	}

	bool InputEventReceiverInterface::CheckButtonPressed(Key button, bool previous_frame)
	{
		if (DoCheckButtonPressed(button, previous_frame))
		{
			if (button.GetType() == KeyType::KEYBOARD)
				SetInputMode(InputMode::KEYBOARD);
			else if (button.GetType() == KeyType::MOUSE)
				SetInputMode(InputMode::MOUSE);
			else if (button.GetType() == KeyType::GAMEPAD)
				SetInputMode(InputMode::GAMEPAD);
			return true;
		}
		return false;
	}

	bool InputEventReceiverInterface::DoCheckButtonPressed(Key button, bool previous_frame)
	{
		return WindowApplication::GetApplicationKeyState(button, previous_frame);
	}

	void InputEventReceiverInterface::OnInputModeChanged(InputMode new_mode, InputMode old_mode) 
	{
	}

	bool InputEventReceiverInterface::OnMouseMove(double x, double y) 
	{
		if (OnMouseMoveImpl(x, y))
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

	bool InputEventReceiverInterface::OnMouseMoveImpl(double x, double y)
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
