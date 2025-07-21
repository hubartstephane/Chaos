#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool InputDeviceUserInterface::EnumerateDeviceHierarchy(LightweightFunction<bool(InputDeviceUserInterface const *)> func) const
	{
		return func(this);
	}

	KeyState const * InputDeviceUserInterface::GetKeyState(Key key) const
	{
		KeyState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, &key](InputDeviceUserInterface const * in_device_user) 
		{
			result = in_device_user->DoGetKeyState(key);
			return (result != nullptr); // continue until some result is found
		});
		return result;
	}

	AxisState const * InputDeviceUserInterface::GetAxisState(GamepadAxis axis) const
	{
		AxisState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, &axis](InputDeviceUserInterface const * in_device_user)
		{
			result = in_device_user->DoGetAxisState(axis);
			return (result != nullptr); // continue until some result is found
		});
		return result;
	}

	StickState const * InputDeviceUserInterface::GetStickState(GamepadStick stick) const
	{	
		StickState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, &stick](InputDeviceUserInterface const * in_device_user)
		{
			result = in_device_user->DoGetStickState(stick);
			return (result != nullptr); // continue until some result is found
		});
		return result;
	}

	bool InputDeviceUserInterface::ForAllKeys(LightweightFunction<bool(Key, KeyState const &)> func) const
	{
		return EnumerateDeviceHierarchy([this, &func](InputDeviceUserInterface const * in_device_user)
		{
			return in_device_user->DoForAllKeys(func);
		});
	}

	bool InputDeviceUserInterface::ForAllAxes(LightweightFunction<bool(GamepadAxis, AxisState const &)> func) const
	{
		return EnumerateDeviceHierarchy([this, &func](InputDeviceUserInterface const * in_device_user)
		{
			return in_device_user->DoForAllAxes(func);
		});
	}

	bool InputDeviceUserInterface::ForAllSticks(LightweightFunction<bool(GamepadStick, StickState const &)> func) const
	{
		return EnumerateDeviceHierarchy([this, &func](InputDeviceUserInterface const * in_device_user)
		{
			return in_device_user->DoForAllSticks(func);
		});
	}

	KeyState const * InputDeviceUserInterface::DoGetKeyState(Key key) const
	{
		return nullptr;
	}

	AxisState const * InputDeviceUserInterface::DoGetAxisState(GamepadAxis axis) const
	{
		return nullptr;
	}

	StickState const * InputDeviceUserInterface::DoGetStickState(GamepadStick stick) const
	{	
		return nullptr;
	}

	bool InputDeviceUserInterface::DoForAllKeys(LightweightFunction<bool(Key, KeyState const &)> func) const
	{
		return false;
	}

	bool InputDeviceUserInterface::DoForAllAxes(LightweightFunction<bool(GamepadAxis, AxisState const &)> func) const
	{
		return false;
	}

	bool InputDeviceUserInterface::DoForAllSticks(LightweightFunction<bool(GamepadStick, StickState const &)> func) const
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
