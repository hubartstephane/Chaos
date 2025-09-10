#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool InputDeviceInterface::EnumerateDeviceHierarchy(LightweightFunction<bool(InputDeviceInterface const *)> func) const
	{
		return func(this);
	}

	KeyState const * InputDeviceInterface::GetKeyState(Key key) const
	{
		KeyState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, &key](InputDeviceInterface const * in_input_device) 
		{
			result = in_input_device->DoGetKeyState(key);
			return (result != nullptr); // continue until some result is found
		});
		return result;
	}

	AxisState const * InputDeviceInterface::GetAxisState(GamepadAxis axis) const
	{
		AxisState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, &axis](InputDeviceInterface const * in_input_device)
		{
			result = in_input_device->DoGetAxisState(axis);
			return (result != nullptr); // continue until some result is found
		});
		return result;
	}

	StickState const * InputDeviceInterface::GetStickState(GamepadStick stick) const
	{	
		StickState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, &stick](InputDeviceInterface const * in_input_device)
		{
			result = in_input_device->DoGetStickState(stick);
			return (result != nullptr); // continue until some result is found
		});
		return result;
	}

	bool InputDeviceInterface::ForAllKeys(LightweightFunction<bool(Key, KeyState const &)> func) const
	{
		return EnumerateDeviceHierarchy([this, &func](InputDeviceInterface const * in_input_device)
		{
			return in_input_device->DoForAllKeys(func);
		});
	}

	bool InputDeviceInterface::ForAllAxes(LightweightFunction<bool(GamepadAxis, AxisState const &)> func) const
	{
		return EnumerateDeviceHierarchy([this, &func](InputDeviceInterface const * in_input_device)
		{
			return in_input_device->DoForAllAxes(func);
		});
	}

	bool InputDeviceInterface::ForAllSticks(LightweightFunction<bool(GamepadStick, StickState const &)> func) const
	{
		return EnumerateDeviceHierarchy([this, &func](InputDeviceInterface const * in_input_device)
		{
			return in_input_device->DoForAllSticks(func);
		});
	}

	KeyState const * InputDeviceInterface::DoGetKeyState(Key key) const
	{
		return nullptr;
	}

	AxisState const * InputDeviceInterface::DoGetAxisState(GamepadAxis axis) const
	{
		return nullptr;
	}

	StickState const * InputDeviceInterface::DoGetStickState(GamepadStick stick) const
	{	
		return nullptr;
	}

	bool InputDeviceInterface::DoForAllKeys(LightweightFunction<bool(Key, KeyState const &)> func) const
	{
		return false;
	}

	bool InputDeviceInterface::DoForAllAxes(LightweightFunction<bool(GamepadAxis, AxisState const &)> func) const
	{
		return false;
	}

	bool InputDeviceInterface::DoForAllSticks(LightweightFunction<bool(GamepadStick, StickState const &)> func) const
	{
		return false;
	}

	bool InputDeviceInterface::GetKeyValue(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->GetValue();
		return false;
	}

	KeyStatus InputDeviceInterface::GetKeyStatus(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->GetStatus();
		return KeyStatus::NONE;
	}

	bool InputDeviceInterface::IsKeyDown(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->IsDown();
		return false;
	}

	bool InputDeviceInterface::IsKeyUp(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->IsUp();
		return true;
	}

	bool InputDeviceInterface::IsKeyJustPressed(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->IsJustPressed();
		return false;
	}

	bool InputDeviceInterface::IsKeyJustReleased(Key key) const
	{
		if (KeyState const * key_state = GetKeyState(key))
			return key_state->IsJustReleased();
		return false;
	}

	float InputDeviceInterface::GetAxisValue(GamepadAxis axis) const
	{
		if (AxisState const * axis_state = GetAxisState(axis))
			return axis_state->GetValue();
		return 0.0f;
	}

	glm::vec2 InputDeviceInterface::GetStickValue(GamepadStick stick) const
	{
		if (StickState const * stick_state = GetStickState(stick))
			return stick_state->GetValue();
		return {0.0f, 0.0f};
	}

	bool InputDeviceInterface::IsAnyKeyAction() const
	{
		return ForAllKeys([](Key key, KeyState const & state)
		{
			return state.IsDown();
		});
	}

	bool InputDeviceInterface::IsAnyAxisAction() const
	{
		return ForAllAxes([](GamepadAxis axis, AxisState const & state)
		{
			return (state.GetValue() != 0.0f);
		});
	}

	bool InputDeviceInterface::IsAnyAction() const
	{
		return IsAnyKeyAction() || IsAnyAxisAction();
	}

	bool InputDeviceInterface::IsAnyKeyJustPressed() const
	{
		return ForAllKeys([](Key key, KeyState const & state)
		{
			return state.IsJustPressed();
		});
	}

}; // namespace chaos
