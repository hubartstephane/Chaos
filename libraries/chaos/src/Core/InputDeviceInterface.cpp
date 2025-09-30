#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool InputDeviceInterface::EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const
	{
		return func(this);
	}

	KeyState const * InputDeviceInterface::GetInputState(Key key) const
	{
		KeyState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, &key](InputDeviceInterface const * in_input_device) 
		{
			result = in_input_device->DoGetInputState(key);
			return (result != nullptr); // continue until some result is found
		});
		return result;
	}

	AxisState const * InputDeviceInterface::GetInputState(GamepadAxis axis) const
	{
		AxisState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, &axis](InputDeviceInterface const * in_input_device)
		{
			result = in_input_device->DoGetInputState(axis);
			return (result != nullptr); // continue until some result is found
		});
		return result;
	}

	StickState const * InputDeviceInterface::GetInputState(GamepadStick stick) const
	{	
		StickState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, &stick](InputDeviceInterface const * in_input_device)
		{
			result = in_input_device->DoGetInputState(stick);
			return (result != nullptr); // continue until some result is found
		});
		return result;
	}

	bool InputDeviceInterface::ForAllKeys(ForAllKeysFunction func) const
	{
		return EnumerateDeviceHierarchy([this, &func](InputDeviceInterface const * in_input_device)
		{
			return in_input_device->DoForAllKeys(func);
		});
	}

	bool InputDeviceInterface::ForAllAxes(ForAllAxesFunction func) const
	{
		return EnumerateDeviceHierarchy([this, &func](InputDeviceInterface const * in_input_device)
		{
			return in_input_device->DoForAllAxes(func);
		});
	}

	bool InputDeviceInterface::ForAllSticks(ForAllSticksFunction func) const
	{
		return EnumerateDeviceHierarchy([this, &func](InputDeviceInterface const * in_input_device)
		{
			return in_input_device->DoForAllSticks(func);
		});
	}

	KeyState const * InputDeviceInterface::DoGetInputState(Key key) const
	{
		return nullptr;
	}

	AxisState const * InputDeviceInterface::DoGetInputState(GamepadAxis axis) const
	{
		return nullptr;
	}

	StickState const * InputDeviceInterface::DoGetInputState(GamepadStick stick) const
	{	
		return nullptr;
	}

	bool InputDeviceInterface::DoForAllKeys(ForAllKeysFunction func) const
	{
		return false;
	}

	bool InputDeviceInterface::DoForAllAxes(ForAllAxesFunction func) const
	{
		return false;
	}

	bool InputDeviceInterface::DoForAllSticks(ForAllSticksFunction func) const
	{
		return false;
	}

	bool InputDeviceInterface::GetInputValue(Key key) const
	{
		if (KeyState const * key_state = GetInputState(key))
			return key_state->GetValue();
		return false;
	}

	KeyStatus InputDeviceInterface::GetKeyStatus(Key key) const
	{
		if (KeyState const * key_state = GetInputState(key))
			return key_state->GetStatus();
		return KeyStatus::NONE;
	}

	bool InputDeviceInterface::IsKeyDown(Key key) const
	{
		if (KeyState const * key_state = GetInputState(key))
			return key_state->IsDown();
		return false;
	}

	bool InputDeviceInterface::IsKeyUp(Key key) const
	{
		if (KeyState const * key_state = GetInputState(key))
			return key_state->IsUp();
		return true;
	}

	bool InputDeviceInterface::IsKeyJustPressed(Key key) const
	{
		if (KeyState const * key_state = GetInputState(key))
			return key_state->IsJustPressed();
		return false;
	}

	bool InputDeviceInterface::IsKeyJustReleased(Key key) const
	{
		if (KeyState const * key_state = GetInputState(key))
			return key_state->IsJustReleased();
		return false;
	}

	float InputDeviceInterface::GetInputValue(GamepadAxis axis) const
	{
		if (AxisState const * axis_state = GetInputState(axis))
			return axis_state->GetValue();
		return 0.0f;
	}

	glm::vec2 InputDeviceInterface::GetInputValue(GamepadStick stick) const
	{
		if (StickState const * stick_state = GetInputState(stick))
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
