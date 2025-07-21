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

	ButtonState const * GamepadState::GetButtonState(GamepadButton button) const
	{
		if (button == GamepadButton::UNKNOWN)
			return nullptr;
		return &buttons[(size_t)button];
	}

	AxisState const *GamepadState::GetAxisState(GamepadAxis axis) const
	{
		if (axis == GamepadAxis::UNKNOWN)
			return nullptr;
		return &axes[(size_t)axis];
	}

	StickState const *GamepadState::GetStickState(GamepadStick stick) const
	{	
		if (stick == GamepadStick::UNKNOWN)
			return nullptr;
		return &sticks[(size_t)stick];
	}

	bool GamepadState::ForAllButtons(LightweightFunction<bool(GamepadButton, ButtonState const &)> func) const
	{
		for (int i = 0 ; i < buttons.size() ; ++i)
			if (func(GamepadButton(i), buttons[i]))
				return true;
		return false;
	}
	bool GamepadState::ForAllAxes(LightweightFunction<bool(GamepadAxis, AxisState const &)> func) const
	{
		for (int i = 0 ; i < axes.size() ; ++i)
			if (func(GamepadAxis(i), axes[i]))
				return true;
		return false;
	}
	bool GamepadState::ForAllSticks(LightweightFunction<bool(GamepadStick, StickState const &)> func) const
	{
		for (int i = 0 ; i < sticks.size() ; ++i)
			if (func(GamepadStick(i), sticks[i]))
				return true;
		return false;
	}

	void GamepadState::UpdateAxisAndButtons(int stick_index, float dead_zone)
	{
		GLFWgamepadstate state;
		glfwGetGamepadState(stick_index, &state);

		// update axes
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

		// update standard buttons
		for (size_t i = 0 ; i < BUTTON_COUNT ; ++i)
		{
			buttons[i].SetValue(state.buttons[i] != 0);
		}

		// update virtual buttons
		auto UpdateVirtualButton = [this](GamepadButton dst_button, GamepadAxis src_axis)
		{
			if (AxisState const * axis_state = GetAxisState(src_axis))
			{
				ButtonState button_state;
				button_state.value = axis_state->value != 0.0f;
				button_state.update_time = axis_state->update_time;
				buttons[int(dst_button)] = button_state;
			}
		};

		UpdateVirtualButton(GamepadButton::LEFT_TRIGGER, GamepadAxis::LEFT_TRIGGER);
		UpdateVirtualButton(GamepadButton::RIGHT_TRIGGER, GamepadAxis::RIGHT_TRIGGER);

		// update sticks
		auto UpdateVirtualStick = [&](GamepadStick dst_stick, GamepadAxis src_horizontal_axis, GamepadAxis src_vertical_axis)
		{
			AxisState const * horizontal_axis_state = GetAxisState(src_horizontal_axis);
			if (horizontal_axis_state == nullptr)
				return;
			AxisState const * vertical_axis_state = GetAxisState(src_vertical_axis);
			if (vertical_axis_state == nullptr)
				return;

			glm::vec2 stick_value = 
			{
				horizontal_axis_state->GetValue(),
				vertical_axis_state->GetValue()
			};

			// if the length is greater than 1, renormalize it to 1.0f !
			float sqr_len = stick_value.x * stick_value.x + stick_value.y * stick_value.y;
			if (sqr_len > 1.0f)
			{
				float len = std::sqrt(sqr_len);
				stick_value.x /= len;
				stick_value.y /= len;
			}

			StickState stick_state;
			stick_state.value = stick_value;
			stick_state.min_value = {horizontal_axis_state->min_value, vertical_axis_state->min_value};
			stick_state.max_value = {horizontal_axis_state->max_value, vertical_axis_state->max_value};
			stick_state.update_time = std::max(horizontal_axis_state->update_time, vertical_axis_state->update_time);
			sticks[int(dst_stick)] = stick_state;
		};

		UpdateVirtualStick(GamepadStick::LEFT_STICK, GamepadAxis::LEFT_AXIS_X, GamepadAxis::LEFT_AXIS_Y);
		UpdateVirtualStick(GamepadStick::RIGHT_STICK, GamepadAxis::RIGHT_AXIS_X, GamepadAxis::RIGHT_AXIS_X);
	}

}; // namespace chaos
