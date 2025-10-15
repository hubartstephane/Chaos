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

	KeyState const * GamepadState::DoGetInputState(Key input) const
	{
		if (!IsGamepadInput(input))
			return nullptr;
		return &buttons[size_t(input) - size_t(Key::GAMEPAD_FIRST)];
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

	float GamepadState::ClampAndNormalizeInput1D(float value, float dead_zone, float max_zone) const
	{
		if (dead_zone >= 0.0f && max_zone > dead_zone)
		{
			if (value <= dead_zone && value >= -dead_zone)
			{
				value = 0.0f;
			}
			else if (value > 0.0f)
			{
				value = (value - dead_zone) / (max_zone - dead_zone);
				if (value > 1.0f)
					value = 1.0f;
			}
			else if (value < 0.0f)
			{
				value = (value + dead_zone) / (max_zone - dead_zone);
				if (value < -1.0f)
					value = -1.0f;
			}
		}
		return value;
	}

	glm::vec2 GamepadState::ClampAndNormalizeInput2D(glm::vec2 value, float dead_zone, float max_zone) const
	{
		if (dead_zone >= 0.0f && max_zone > dead_zone)
		{
			float value_length = glm::length(value);

			if (value_length <= dead_zone)
			{
				value = { 0.0f, 0.0f };
			}
			else if (value_length >= max_zone)
			{
				value /= value_length;
			}
			else
			{
				value =
					(value / value_length) *
					(value_length - dead_zone) / (max_zone - dead_zone);
			}
		}
		return value;
	}

	// XXX: Not all devices are perfect (rest value may not be 0 and max value may be greater than 1)
	//      Use a [dead_zone, max_zone] range for clamping and renormalization

	void GamepadState::UpdateAxisAndButtons(int stick_index, float dead_zone, float max_zone)
	{
		GLFWgamepadstate state;
		glfwGetGamepadState(stick_index, &state);

		// update axes
		for (size_t i = 0; i < AXIS_COUNT; ++i)
		{
			Input1D input = Input1D(i + size_t(Input1D::GAMEPAD_FIRST));

			float & value = state.axes[i];
			// renormalize icomming value [-1 .. +1] => [0 .. 1]
			if (input == Input1D::GAMEPAD_LEFT_TRIGGER || input == Input1D::GAMEPAD_RIGHT_TRIGGER)
				value = (value * 0.5f + 0.5f);
			// want positive Y when stick is UP
			else if (input == Input1D::GAMEPAD_LEFT_AXIS_Y || input == Input1D::GAMEPAD_RIGHT_AXIS_Y)
				value = -value;
			axes[i].SetValue(ClampAndNormalizeInput1D(value, dead_zone, max_zone));
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
				bool value = (axis_state->value != 0.0f);
				buttons[size_t(dst_button) - size_t(Key::GAMEPAD_FIRST)].SetValue(value);
			}
		};

		UpdateVirtualButton(Key::GAMEPAD_LEFT_TRIGGER, Input1D::GAMEPAD_LEFT_TRIGGER);
		UpdateVirtualButton(Key::GAMEPAD_RIGHT_TRIGGER, Input1D::GAMEPAD_RIGHT_TRIGGER);

		// update sticks
		auto UpdateVirtualStick = [&](Input2D dst_stick, Input1D src_horizontal_axis, Input1D src_vertical_axis)
		{
			glm::vec2 stick_value = 
			{
				state.axes[size_t(src_horizontal_axis) - size_t(Input1D::GAMEPAD_FIRST)],
				state.axes[size_t(src_vertical_axis) - size_t(Input1D::GAMEPAD_FIRST)]
			};
			sticks[size_t(dst_stick) - size_t(Input2D::GAMEPAD_FIRST)].SetValue(ClampAndNormalizeInput2D(stick_value, dead_zone, max_zone));
		};

		UpdateVirtualStick(Input2D::GAMEPAD_LEFT_STICK, Input1D::GAMEPAD_LEFT_AXIS_X, Input1D::GAMEPAD_LEFT_AXIS_Y);
		UpdateVirtualStick(Input2D::GAMEPAD_RIGHT_STICK, Input1D::GAMEPAD_RIGHT_AXIS_X, Input1D::GAMEPAD_RIGHT_AXIS_Y);
	}

}; // namespace chaos
