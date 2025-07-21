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


	#if 0

	bool InputDeviceUserInterface::IsAnyButtonAction() const
	{
		for (ButtonState const& b : buttons)
			if (b.GetValue())
				return true;
		return false;
	}

	bool InputDeviceUserInterface::IsAnyAxisAction() const
	{
		for (AxisState const& a : axes)
			if (a.GetValue() != 0.0f)
				return true;
		return false;
	}

	bool InputDeviceUserInterface::IsAnyAction() const
	{
		return IsAnyButtonAction() || IsAnyAxisAction();
	}

	bool InputDeviceUserInterface::IsAnyButtonJustPressed() const
	{
		for (ButtonState const& b : buttons)
			if (b.IsJustPressed())
				return true;
		return false;
	}
	#endif

}; // namespace chaos
