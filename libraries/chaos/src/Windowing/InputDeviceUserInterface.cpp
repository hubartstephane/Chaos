#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ButtonState const * InputDeviceUserInterface::GetButtonState(GamepadButton button) const
	{
		return nullptr;
	}

	AxisState const * InputDeviceUserInterface::GetAxisState(GamepadAxis axis) const
	{
		return nullptr;
	}

	StickState const * InputDeviceUserInterface::GetStickState(GamepadStick stick) const
	{	
		return nullptr;
	}

	bool InputDeviceUserInterface::ForAllButtons(LightweightFunction<bool(GamepadButton, ButtonState const &)> func) const
	{
		return false;
	}

	bool InputDeviceUserInterface::ForAllAxes(LightweightFunction<bool(GamepadAxis, AxisState const &)> func) const
	{
		return false;
	}

	bool InputDeviceUserInterface::ForAllSticks(LightweightFunction<bool(GamepadStick, StickState const &)> func) const
	{
		return false;
	}

	bool InputDeviceUserInterface::GetButtonValue(GamepadButton button) const
	{
		if (ButtonState const * button_state = GetButtonState(button))
			return button_state->GetValue();
		return false;
	}

	ButtonStatus InputDeviceUserInterface::GetButtonStatus(GamepadButton button) const
	{
		if (ButtonState const * button_state = GetButtonState(button))
			return button_state->GetStatus();
		return ButtonStatus::NONE;
	}

	bool InputDeviceUserInterface::IsButtonDown(GamepadButton button) const
	{
		if (ButtonState const * button_state = GetButtonState(button))
			return button_state->IsDown();
		return false;
	}

	bool InputDeviceUserInterface::IsButtonUp(GamepadButton button) const
	{
		if (ButtonState const * button_state = GetButtonState(button))
			return button_state->IsUp();
		return true;
	}

	bool InputDeviceUserInterface::IsButtonJustPressed(GamepadButton button) const
	{
		if (ButtonState const * button_state = GetButtonState(button))
			return button_state->IsJustPressed();
		return false;
	}

	bool InputDeviceUserInterface::IsButtonJustReleased(GamepadButton button) const
	{
		if (ButtonState const * button_state = GetButtonState(button))
			return button_state->IsJustReleased();
		return false;
	}

	float InputDeviceUserInterface::GetAxisValue(GamepadAxis axis) const
	{
		if (AxisState const * axis_state = GetAxisState(axis))
			return axis_state->GetValue();
		return 0.0f;
	}

	glm::vec2 InputDeviceUserInterface::GetStickValue(GamepadStick stick) const
	{
		if (StickState const * stick_state = GetStickState(stick))
			return stick_state->GetValue();
		return {0.0f, 0.0f};
	}

	bool InputDeviceUserInterface::IsAnyButtonAction() const
	{
		return ForAllButtons([](GamepadButton button, ButtonState const & state)
		{
			return state.IsDown();
		});
	}

	bool InputDeviceUserInterface::IsAnyAxisAction() const
	{
		return ForAllAxes([](GamepadAxis axis, AxisState const & state)
		{
			return (state.GetValue() != 0.0f);
		});
	}

	bool InputDeviceUserInterface::IsAnyAction() const
	{
		return IsAnyButtonAction() || IsAnyAxisAction();
	}

	bool InputDeviceUserInterface::IsAnyButtonJustPressed() const
	{
		return ForAllButtons([](GamepadButton button, ButtonState const & state)
		{
			return state.IsJustPressed();
		});
	}

}; // namespace chaos
