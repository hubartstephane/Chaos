#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void GamepadState::Clear()
	{
		for (ButtonState& b : buttons)
			b.Clear();
		for (AxisState& a : axes)
			a.Clear();
	}

	size_t GamepadState::GetButtonCount() const
	{
		return buttons.size();
	}

	size_t GamepadState::GetAxisCount() const
	{
		return axes.size();
	}

	ButtonState GamepadState::GetButtonState(GamepadButton button) const
	{
		// early exit
		if (button == GamepadButton::UNKNOWN)
			return {};

		// virtual buttons
		if (button == GamepadButton::LEFT_TRIGGER)
		{
			AxisState axis_state = GetAxisState(GamepadAxis::LEFT_TRIGGER);

			ButtonState result;
			result.value = axis_state.value != 0.0f;
			result.update_time = axis_state.update_time;
			return result;
		}

		if (button == GamepadButton::RIGHT_TRIGGER)
		{
			AxisState axis_state = GetAxisState(GamepadAxis::RIGHT_TRIGGER);

			ButtonState result;
			result.value = axis_state.value != 0.0f;
			result.update_time = axis_state.update_time;
			return result;
		}
		// standard input
		return buttons[(size_t)button];
	}

	AxisState GamepadState::GetAxisState(GamepadAxis axis) const
	{
		// early exit
		if (axis == GamepadAxis::UNKNOWN)
			return {};
		// standard input
		return axes[(size_t)axis];
	}

	StickState GamepadState::GetStickState(GamepadStick stick) const
	{	
		// early exit
		if (stick == GamepadStick::UNKNOWN)
			return {};

		// get value
		auto GetClampedAxis = [&](GamepadAxis horizontal_axis, GamepadAxis vertical_axis)
		{
			AxisState horizontal_axis_state = GetAxisState(horizontal_axis);
			AxisState vertical_axis_state = GetAxisState(vertical_axis);

			glm::vec2 stick_value = 
			{
				horizontal_axis_state.GetValue(),
				vertical_axis_state.GetValue()
			};

			// if the length is greater than 1, renormalize it to 1.0f !
			float sqr_len = stick_value.x * stick_value.x + stick_value.y * stick_value.y;
			if (sqr_len > 1.0f)
			{
				float len = std::sqrt(sqr_len);
				stick_value.x /= len;
				stick_value.y /= len;
			}

			StickState result;
			result.value = stick_value;
			result.min_value = {horizontal_axis_state.min_value, vertical_axis_state.min_value};
			result.max_value = {horizontal_axis_state.max_value, vertical_axis_state.max_value};
			result.update_time = std::max(horizontal_axis_state.update_time, vertical_axis_state.update_time);
			return result;
		};

		if (stick == GamepadStick::LEFT_STICK)
		{
			return GetClampedAxis(GamepadAxis::LEFT_AXIS_X, GamepadAxis::LEFT_AXIS_Y);
		}
		else if (stick == GamepadStick::RIGHT_STICK)
		{
			return GetClampedAxis(GamepadAxis::RIGHT_AXIS_X, GamepadAxis::RIGHT_AXIS_X);
		}
		return {};
	}

	bool GamepadState::GetButtonValue(GamepadButton button) const
	{
		return GetButtonState(button).GetValue();
	}

	ButtonStatus GamepadState::GetButtonStatus(GamepadButton button) const
	{
		return GetButtonState(button).GetStatus();
	}

	bool GamepadState::IsButtonDown(GamepadButton button) const
	{
		return GetButtonState(button).IsDown();
	}

	bool GamepadState::IsButtonUp(GamepadButton button) const
	{
		return GetButtonState(button).IsUp();
	}

	bool GamepadState::IsButtonJustPressed(GamepadButton button) const
	{
		return GetButtonState(button).IsJustPressed();
	}

	bool GamepadState::IsButtonJustReleased(GamepadButton button) const
	{
		return GetButtonState(button).IsJustReleased();
	}

	float GamepadState::GetAxisValue(GamepadAxis axis) const
	{
		return GetAxisState(axis).GetValue();
	}

	glm::vec2 GamepadState::GetStickValue(GamepadStick stick) const
	{
		return GetStickState(stick).GetValue();
	}

	bool GamepadState::IsAnyButtonAction() const
	{
		for (ButtonState const& b : buttons)
			if (b.GetValue())
				return true;
		return false;
	}

	bool GamepadState::IsAnyAxisAction() const
	{
		for (AxisState const& a : axes)
			if (a.GetValue() != 0.0f)
				return true;
		return false;
	}

	bool GamepadState::IsAnyAction() const
	{
		return IsAnyButtonAction() || IsAnyAxisAction();
	}

	bool GamepadState::IsAnyButtonJustPressed() const
	{
		for (ButtonState const& b : buttons)
			if (b.IsJustPressed())
				return true;
		return false;
	}

	void GamepadState::UpdateAxisAndButtons(int stick_index, float dead_zone)
	{
		GLFWgamepadstate state;
		glfwGetGamepadState(stick_index, &state);

		for (size_t i = 0; i < AXIS_COUNT; ++i)
		{
			float value = state.axes[i];
			// renormalize icomming value [-1 .. +1] => [0 .. 1]
			if (i == (size_t)GamepadAxis::LEFT_TRIGGER || i == (size_t)GamepadAxis::RIGHT_TRIGGER)
				value = (value * 0.5f + 0.5f);
			// want positive Y when stick is UP
			else if (i == (size_t)GamepadAxis::LEFT_AXIS_Y || i == (size_t)GamepadAxis::RIGHT_AXIS_Y)
				value = -value;
			axes[i].SetValue(value, dead_zone);
		}

		for (size_t i = 0 ; i < BUTTON_COUNT ; ++i)
		{
			buttons[i].SetValue(state.buttons[i] != 0);
		}
	}

}; // namespace chaos
