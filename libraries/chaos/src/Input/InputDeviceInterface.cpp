#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool InputDeviceInterface::EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const
	{
		return func(this);
	}

	KeyState const * InputDeviceInterface::GetInputState(Key input) const
	{
		KeyState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, input](InputDeviceInterface const * in_input_device)
		{
			result = in_input_device->DoGetInputState(input);
			return (result != nullptr); // continue until some result is found
		});
		return result;
	}

	Input1DState const * InputDeviceInterface::GetInputState(Input1D input) const
	{
		Input1DState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, input](InputDeviceInterface const * in_input_device)
		{
			result = in_input_device->DoGetInputState(input);
			return (result != nullptr); // continue until some result is found
		});
		return result;
	}

	Input2DState const * InputDeviceInterface::GetInputState(Input2D input) const
	{	
		Input2DState const * result = nullptr;
		EnumerateDeviceHierarchy([this, &result, input](InputDeviceInterface const * in_input_device)
		{
			result = in_input_device->DoGetInputState(input);
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

	bool InputDeviceInterface::ForAllInput1D(ForAllInput1DFunction func) const
	{
		return EnumerateDeviceHierarchy([this, &func](InputDeviceInterface const * in_input_device)
		{
			return in_input_device->DoForAllInput1D(func);
		});
	}

	bool InputDeviceInterface::ForAllInput2D(ForAllInput2DFunction func) const
	{
		return EnumerateDeviceHierarchy([this, &func](InputDeviceInterface const * in_input_device)
		{
			return in_input_device->DoForAllInput2D(func);
		});
	}

	KeyState const * InputDeviceInterface::DoGetInputState(Key input) const
	{
		return nullptr;
	}

	Input1DState const * InputDeviceInterface::DoGetInputState(Input1D input) const
	{
		return nullptr;
	}

	Input2DState const * InputDeviceInterface::DoGetInputState(Input2D input) const
	{	
		return nullptr;
	}

	bool InputDeviceInterface::DoForAllKeys(ForAllKeysFunction func) const
	{
		return false;
	}

	bool InputDeviceInterface::DoForAllInput1D(ForAllInput1DFunction func) const
	{
		return false;
	}

	bool InputDeviceInterface::DoForAllInput2D(ForAllInput2DFunction func) const
	{
		return false;
	}

	bool InputDeviceInterface::GetInputValue(Key input) const
	{
		if (KeyState const * input_state = GetInputState(input))
			return input_state->GetValue();
		return false;
	}

	float InputDeviceInterface::GetInputValue(Input1D input) const
	{
		if (Input1DState const* input_state = GetInputState(input))
			return input_state->GetValue();
		return 0.0f;
	}

	glm::vec2 InputDeviceInterface::GetInputValue(Input2D input) const
	{
		if (Input2DState const* input_state = GetInputState(input))
			return input_state->GetValue();
		return { 0.0f, 0.0f };
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

	bool InputDeviceInterface::IsAnyKeyAction() const
	{
		return ForAllKeys([](Key key, KeyState const & state)
		{
			return state.IsDown();
		});
	}

	bool InputDeviceInterface::IsAnyAxisAction() const
	{
		return ForAllInput1D([](Input1D input, Input1DState const & state)
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
