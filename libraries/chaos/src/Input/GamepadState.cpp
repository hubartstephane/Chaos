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

	float GamepadState::ClampAndNormalizeInput1D(float value, GamepadInputFilterSettings const& in_filter_settings) const
	{
		if (in_filter_settings.dead_zone >= 0.0f && in_filter_settings.max_zone >= in_filter_settings.dead_zone)
		{
			if (value <= in_filter_settings.dead_zone && value >= -in_filter_settings.dead_zone)
				return 0.0f;
			
			if (value > 0.0f)
				value = (value - in_filter_settings.dead_zone) / (in_filter_settings.max_zone - in_filter_settings.dead_zone);
			else if (value < 0.0f)
				value = (value + in_filter_settings.dead_zone) / (in_filter_settings.max_zone - in_filter_settings.dead_zone);
		}
		return Interpolate(in_filter_settings.length_interpolation_type, std::clamp(value, -1.0f, +1.0f), 0.0f, 1.0f);
	}

	glm::vec2 GamepadState::ClampAndNormalizeInput2D(glm::vec2 value, GamepadInputFilterSettings const& in_filter_settings) const
	{
		if (in_filter_settings.dead_zone >= 0.0f && in_filter_settings.max_zone > in_filter_settings.dead_zone)
		{
			float value_length = glm::length(value);

			if (value_length <= in_filter_settings.dead_zone)
				return { 0.0f, 0.0f };
			if (value_length >= in_filter_settings.max_zone)
				return value / value_length;

			float new_unfiltered_length = (value_length - in_filter_settings.dead_zone) / (in_filter_settings.max_zone - in_filter_settings.dead_zone);
			float filtered_length = Interpolate(in_filter_settings.length_interpolation_type, new_unfiltered_length, 0.0f, 1.0f);
			return (value / value_length) * filtered_length;
		}
		return value;
	}

	float GamepadState::SnapInput2DAngleToSectorBoundaries(float angle, GamepadInputFilterSettings const& in_filter_settings) const
	{
		assert(angle >= 0.0f);
		assert(angle < 2.0f * float(M_PI));

		// early exit
		if (in_filter_settings.sector_snap_angle_ratio <= 0.0f)
			return angle;
		if (in_filter_settings.sector_snap_count <= 1)
			return angle;
		float sector_snap_angle_ratio = std::min(in_filter_settings.sector_snap_angle_ratio, 1.0f);

		// find the sector the stick belongs to
		float sector_angle      = 2.0f * float(M_PI) / float(in_filter_settings.sector_snap_count);
		float sector_snap_angle = sector_angle * 0.5f * sector_snap_angle_ratio; // at most, the attraction of the axis is at half the sector size
		int   sector            = int(angle / sector_angle);

		// snap the angle to sector boundaries
		float sector_start_angle = float(sector) * sector_angle;
		float sector_end_angle   = float(sector + 1) * sector_angle;

		if (angle < sector_start_angle + sector_snap_angle)
			return sector_start_angle;
		if (angle > sector_end_angle - sector_snap_angle)
			return sector_end_angle;

		return 
			sector_start_angle + 
			sector_angle * (angle - sector_start_angle - sector_snap_angle) / (sector_angle - 2.0f * sector_snap_angle);
	}

	// XXX: Not all devices are perfect (rest value may not be 0 and max value may be greater than 1)
	//      Use a [dead_zone, max_zone] range for clamping and renormalization

	void GamepadState::UpdateAxisAndButtons(int stick_index, GamepadInputFilterSettings const& in_filter_settings)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(stick_index, &state) == GLFW_FALSE)
		{
			Clear();
			return;
		}

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
			axes[i].SetValue(ClampAndNormalizeInput1D(value, in_filter_settings));
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

			// renormalize stick length so its length is always [0..1] (taking into account dead_zone & max_zone)
			stick_value = ClampAndNormalizeInput2D(stick_value, in_filter_settings);

			// tweak the stick angle to have a slight snapping along sector direction	
			float stick_length = glm::length(stick_value);
			float stick_alpha  = std::atan2(stick_value.y, stick_value.x);
			if (stick_length > 0.0f)
				stick_alpha = SnapInput2DAngleToSectorBoundaries(stick_alpha + float(M_PI), in_filter_settings) - float(M_PI); // +/- pi so that stick_alpha is in range [0..2.pi] rather than [-pi..+pi]

			// reconstitue the new stick value and apply it to gamepad state
			stick_value = 
			{
				stick_length * std::cos(stick_alpha),
				stick_length * std::sin(stick_alpha)
			};

			sticks[size_t(dst_stick) - size_t(Input2D::GAMEPAD_FIRST)].SetValue(stick_value);
		};

		UpdateVirtualStick(Input2D::GAMEPAD_LEFT_STICK, Input1D::GAMEPAD_LEFT_AXIS_X, Input1D::GAMEPAD_LEFT_AXIS_Y);
		UpdateVirtualStick(Input2D::GAMEPAD_RIGHT_STICK, Input1D::GAMEPAD_RIGHT_AXIS_X, Input1D::GAMEPAD_RIGHT_AXIS_Y);
	}

}; // namespace chaos
