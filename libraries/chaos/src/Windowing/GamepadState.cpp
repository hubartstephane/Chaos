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

	ButtonStateChange GamepadState::GetButtonStateChange(GamepadButton button) const
	{
		// early exit
		if (button == GamepadButton::UNKNOWN)
			return ButtonStateChange::NONE;
		// pseudo buttons
		if (button == GamepadButton::LEFT_TRIGGER)
		{
			bool previous_state = (GetAxisValue(GamepadAxis::LEFT_TRIGGER, InputStateFrame::PREVIOUS) > 0);
			bool current_state = (GetAxisValue(GamepadAxis::LEFT_TRIGGER, InputStateFrame::CURRENT) > 0);
			if (current_state == previous_state)
				return (current_state) ? ButtonStateChange::STAY_PRESSED : ButtonStateChange::STAY_RELEASED;
			else
				return (current_state) ? ButtonStateChange::BECOME_PRESSED : ButtonStateChange::BECOME_RELEASED;
		}

		if (button == GamepadButton::RIGHT_TRIGGER)
		{
			bool previous_state = (GetAxisValue(GamepadAxis::RIGHT_TRIGGER, InputStateFrame::PREVIOUS) > 0);
			bool current_state = (GetAxisValue(GamepadAxis::RIGHT_TRIGGER, InputStateFrame::CURRENT) > 0);
			if (current_state == previous_state)
				return (current_state) ? ButtonStateChange::STAY_PRESSED : ButtonStateChange::STAY_RELEASED;
			else
				return (current_state) ? ButtonStateChange::BECOME_PRESSED : ButtonStateChange::BECOME_RELEASED;
		}
		// standard input
		return buttons[(size_t)button].GetStateChange();
	}

	bool GamepadState::IsButtonPressed(GamepadButton button, InputStateFrame frame) const
	{
		// early exit
		if (button == GamepadButton::UNKNOWN)
			return false;
		// pseudo buttons
		if (button == GamepadButton::LEFT_TRIGGER)
		{
			float trigger_value = GetAxisValue(GamepadAxis::LEFT_TRIGGER, frame);
			if (trigger_value > 0)
				return true;
			return false;
		}

		if (button == GamepadButton::RIGHT_TRIGGER)
		{
			float trigger_value = GetAxisValue(GamepadAxis::RIGHT_TRIGGER, frame);
			if (trigger_value > 0)
				return true;
			return false;
		}
		// standard input
		return buttons[(size_t)button].GetValue(frame);
	}

	float GamepadState::GetAxisValue(GamepadAxis axis, InputStateFrame frame) const
	{
		if (axis == GamepadAxis::UNKNOWN)
			return 0.0f;
		return axes[(size_t)axis].GetValue(frame);
	}

	bool GamepadState::IsAnyButtonJustPressed() const
	{
		for (ButtonState const& b : buttons)
			if (b.IsJustPressed())
				return true;
		return false;
	}

	bool GamepadState::IsAnyButtonPressed(InputStateFrame frame) const
	{
		for (ButtonState const& b : buttons)
			if (b.GetValue(frame))
				return true;
		return false;
	}

	bool GamepadState::IsAnyAxisAction(InputStateFrame frame) const
	{
		for (AxisState const& a : axes)
			if (a.GetValue(frame) != 0.0f)
				return true;
		return false;
	}

	bool GamepadState::IsAnyAction(InputStateFrame frame) const
	{
		return IsAnyButtonPressed(frame) || IsAnyAxisAction(frame);
	}

	glm::vec2 GamepadState::GetStickValue(GamepadStick stick, InputStateFrame frame) const
	{
		glm::vec2 result(0.0f, 0.0f);

		// early exit
		if (stick == GamepadStick::UNKNOWN)
			return result;
		if (stick == GamepadStick::LEFT_STICK)
		{
			result.x = GetAxisValue(GamepadAxis::LEFT_AXIS_X, frame);
			result.y = GetAxisValue(GamepadAxis::LEFT_AXIS_Y, frame);
		}
		else if (stick == GamepadStick::RIGHT_STICK)
		{
			result.x = GetAxisValue(GamepadAxis::RIGHT_AXIS_X, frame);
			result.y = GetAxisValue(GamepadAxis::RIGHT_AXIS_Y, frame);
		}
		else
			return result;

		// if the length is greater than 1, renormalize it to 1.0f !
		float sqr_len = result.x * result.x + result.y * result.y;
		if (sqr_len > 1.0f)
		{
			float len = std::sqrt(sqr_len);
			result.x /= len;
			result.y /= len;
		}
		return result;
	}

	void GamepadState::UpdateAxisAndButtons(int stick_index, float delta_time, float dead_zone)
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
			axes[i].UpdateTimerAccumulation(delta_time);
		}

		for (size_t i = 0 ; i < BUTTON_COUNT ; ++i)
		{
			buttons[i].SetValue(state.buttons[i] != 0);
			buttons[i].UpdateTimerAccumulation(delta_time);
		}
	}

}; // namespace chaos
