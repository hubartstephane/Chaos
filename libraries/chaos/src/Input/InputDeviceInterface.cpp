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

	KeyState* InputDeviceInterface::GetInputState(Key input)
	{
		InputDeviceInterface const* const_this = this;
		return (KeyState*)const_this->GetInputState(input); // simply remove constness (not very nice, but far simpler)
	}

	Input1DState* InputDeviceInterface::GetInputState(Input1D input)
	{
		InputDeviceInterface const* const_this = this;
		return (Input1DState*)const_this->GetInputState(input); // simply remove constness (not very nice, but far simpler)
	}

	Input2DState* InputDeviceInterface::GetInputState(Input2D input)
	{
		InputDeviceInterface const* const_this = this;
		return (Input2DState*)const_this->GetInputState(input); // simply remove constness (not very nice, but far simpler)
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

	InputStatus InputDeviceInterface::GetInputStatus(Key input) const
	{
		if (KeyState const * input_state = GetInputState(input))
			return input_state->GetStatus();
		return InputStatus::NONE;
	}

	InputStatus InputDeviceInterface::GetInputStatus(Input1D input) const
	{
		if (Input1DState const* input_state = GetInputState(input))
			return input_state->GetStatus();
		return InputStatus::NONE;
	}

	InputStatus InputDeviceInterface::GetInputStatus(Input2D input) const
	{
		if (Input2DState const* input_state = GetInputState(input))
			return input_state->GetStatus();
		return InputStatus::NONE;
	}

	bool InputDeviceInterface::IsAnyKeyActive() const
	{
		return ForAllKeys([](Key key, KeyState const & state)
		{
			return state.IsActive();
		});
	}

	bool InputDeviceInterface::IsAnyInput1DActive() const
	{
		return ForAllInput1D([](Input1D input, Input1DState const & state)
		{
			return state.IsActive();
		});
	}

	bool InputDeviceInterface::IsAnyInput2DActive() const
	{
		return ForAllInput2D([](Input2D input, Input2DState const & state)
		{
			return state.IsActive();
		});
	}

	bool InputDeviceInterface::IsAnyInputActive() const
	{
		return IsAnyKeyActive() || IsAnyInput1DActive() || IsAnyInput2DActive();
	}

	bool InputDeviceInterface::HasAnyKeyJustBecameActive() const
	{
		return ForAllKeys([](Key key, KeyState const & state)
		{
			return HasInputJustBecameActive(&state);
		});
	}

}; // namespace chaos
