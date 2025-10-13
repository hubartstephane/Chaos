#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void GamepadState::Clear()
	{
		for (KeyState& button : buttons)
			button.Clear();
		for (Input1DState& axis : axes)
			axis.Clear();
	}

	size_t GamepadState::GetButtonCount() const
	{
		return buttons.size();
	}

	size_t GamepadState::GetAxisCount() const
	{
		return axes.size();
	}

	KeyState const * GamepadState::DoGetInputState(Key key) const
	{
		if (!IsGamepadInput(key))
			return nullptr;
		return &buttons[size_t(key) - size_t(Key::GAMEPAD_FIRST)];
	}

	Input1DState const *GamepadState::DoGetInputState(Input1D input) const
	{
		if (!IsGamepadInput(input))
			return nullptr;
		return &axes[size_t(input) - size_t(Input1D::GAMEPAD_FIRST)];
	}

	Input2DState const *GamepadState::DoGetInputState(Input2D input) const
	{	
		if (!IsGamepadInput(input))
			return nullptr;
		return &sticks[size_t(input) - size_t(Input2D::GAMEPAD_FIRST)];
	}

	bool GamepadState::DoForAllKeys(ForAllKeysFunction func) const
	{
		for (size_t i = 0 ; i < buttons.size() ; ++i)
			if (func(Key(i + size_t(Key::GAMEPAD_FIRST)), buttons[i]))
				return true;
		return false;
	}
	bool GamepadState::DoForAllInput1D(ForAllInput1DFunction func) const
	{
		for (size_t i = 0 ; i < axes.size() ; ++i)
			if (func(Input1D(i + size_t(Input1D::GAMEPAD_FIRST)), axes[i]))
				return true;
		return false;
	}
	bool GamepadState::DoForAllInput2D(ForAllInput2DFunction func) const
	{
		for (size_t i = 0 ; i < sticks.size() ; ++i)
			if (func(Input2D(i + size_t(Input2D::GAMEPAD_FIRST)), sticks[i]))
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
			Input1D input = Input1D(i + size_t(Input1D::GAMEPAD_FIRST));

			float value = state.axes[i];
			// renormalize icomming value [-1 .. +1] => [0 .. 1]
			if (input == Input1D::GAMEPAD_LEFT_TRIGGER || input == Input1D::GAMEPAD_RIGHT_TRIGGER)
				value = (value * 0.5f + 0.5f);
			// want positive Y when stick is UP
			else if (input == Input1D::GAMEPAD_LEFT_AXIS_Y || input == Input1D::GAMEPAD_RIGHT_AXIS_Y)
				value = -value;
			axes[i].SetValue(value, dead_zone);
		}

		// update standard buttons
		for (size_t i = 0 ; i < BUTTON_COUNT ; ++i)
		{
			buttons[i].SetValue(state.buttons[i] != 0);
		}

		// update virtual buttons
		auto UpdateVirtualButton = [this](Key dst_button, Input1D src_axis)
		{
			if (Input1DState const * axis_state = GetInputState(src_axis))
			{
				KeyState key_state;
				key_state.value = axis_state->value != 0.0f;
				key_state.update_time = axis_state->update_time;
				buttons[size_t(dst_button) - size_t(Key::GAMEPAD_FIRST)] = key_state;
			}
		};

		UpdateVirtualButton(Key::GAMEPAD_LEFT_TRIGGER, Input1D::GAMEPAD_LEFT_TRIGGER);
		UpdateVirtualButton(Key::GAMEPAD_RIGHT_TRIGGER, Input1D::GAMEPAD_RIGHT_TRIGGER);

		// update sticks
		auto UpdateVirtualStick = [&](Input2D dst_stick, Input1D src_horizontal_axis, Input1D src_vertical_axis)
		{
			Input1DState const * horizontal_axis_state = GetInputState(src_horizontal_axis);
			if (horizontal_axis_state == nullptr)
				return;
			Input1DState const * vertical_axis_state = GetInputState(src_vertical_axis);
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

			Input2DState stick_state;
			stick_state.value = stick_value;
			stick_state.min_value = {horizontal_axis_state->min_value, vertical_axis_state->min_value};
			stick_state.max_value = {horizontal_axis_state->max_value, vertical_axis_state->max_value};
			stick_state.update_time = std::max(horizontal_axis_state->update_time, vertical_axis_state->update_time);
			sticks[size_t(dst_stick) - size_t(Input2D::GAMEPAD_FIRST)] = stick_state;
		};

		UpdateVirtualStick(Input2D::GAMEPAD_LEFT_STICK, Input1D::GAMEPAD_LEFT_AXIS_X, Input1D::GAMEPAD_LEFT_AXIS_Y);
		UpdateVirtualStick(Input2D::GAMEPAD_RIGHT_STICK, Input1D::GAMEPAD_RIGHT_AXIS_X, Input1D::GAMEPAD_RIGHT_AXIS_Y);
	}

}; // namespace chaos
