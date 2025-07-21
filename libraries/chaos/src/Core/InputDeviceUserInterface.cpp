#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	KeyState const * InputDeviceUserInterface::GetKeyState(Key key) const
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

	bool InputDeviceUserInterface::ForAllKeys(LightweightFunction<bool(Key, KeyState const &)> func) const
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

	bool InputDeviceUserInterface::GetKeyValue(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->GetValue();
		return false;
	}

	KeyStatus InputDeviceUserInterface::GetKeyStatus(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->GetStatus();
		return KeyStatus::NONE;
	}

	bool InputDeviceUserInterface::IsKeyDown(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->IsDown();
		return false;
	}

	bool InputDeviceUserInterface::IsKeyUp(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->IsUp();
		return true;
	}

	bool InputDeviceUserInterface::IsKeyJustPressed(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->IsJustPressed();
		return false;
	}

	bool InputDeviceUserInterface::IsKeyJustReleased(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->IsJustReleased();
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

	bool InputDeviceUserInterface::IsAnyKeyAction() const
	{
		return ForAllKeys([](Key key, KeyState const & state)
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
		return IsAnyKeyAction() || IsAnyAxisAction();
	}

	bool InputDeviceUserInterface::IsAnyKeyJustPressed() const
	{
		return ForAllKeys([](Key key, KeyState const & state)
		{
			return state.IsJustPressed();
		});
	}

}; // namespace chaos
