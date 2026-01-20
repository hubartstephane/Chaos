#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool InputDeviceInterface::EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const
	{
		return func(this);
	}

	template<InputType INPUT_TYPE> 
	std::optional<InputState_t<INPUT_TYPE>> InputDeviceInterface::GetInputStateHelper(INPUT_TYPE input) const
	{
		if (input == INPUT_TYPE::UNKNOWN)
			return {};

		std::optional<InputState_t<INPUT_TYPE>> result;
		EnumerateDeviceHierarchy([this, &result, input](InputDeviceInterface const * in_input_device)
		{
			result = in_input_device->DoGetInputState(input);
			return result.has_value(); // continue until some result is found
		});
		return result;
	}

	std::optional<KeyState> InputDeviceInterface::GetInputState(Key input) const
	{
		return GetInputStateHelper(input);
	}

	std::optional<Input1DState> InputDeviceInterface::GetInputState(Input1D input) const
	{
		return GetInputStateHelper(input);
	}

	std::optional<Input2DState> InputDeviceInterface::GetInputState(Input2D input) const
	{	
		return GetInputStateHelper(input);
	}

	std::optional<Input1DState> InputDeviceInterface::GetInputState(MappedInput1D input) const
	{
		Input1DState result;

		auto AccumulateKeyState = [&](Key in_key, float delta_value)
		{
			if (in_key == Key::UNKNOWN)
				return true;
			std::optional<KeyState> state = GetInputState(in_key);
			if (!state.has_value())
				return false;
			if (state->GetValue())
				result.value += delta_value;
			result.update_time = std::max(result.update_time, state->update_time); // works even if unitialized (-1.0)
			return true;
		};

		if (!AccumulateKeyState(input.neg_key, -1.0f))
			return {};
		if (!AccumulateKeyState(input.pos_key, +1.0f))
			return {};
		return result;
	}

	std::optional<Input2DState> InputDeviceInterface::GetInputState(MappedInput2D input) const
	{
		Input2DState result;

		auto AccumulateKeyState = [&](Key in_key, int axis, float delta_value)
		{
			if (in_key == Key::UNKNOWN)
				return true;
			std::optional<KeyState> state = GetInputState(in_key);
			if (!state.has_value())
				return false;
			if (state->GetValue())
				result.value[axis] += delta_value;
			result.update_time = std::max(result.update_time, state->update_time); // works even if unitialized (-1.0)
			return true;
		};
		
		if (!AccumulateKeyState(input.left_key,  0, -1.0f))
			return {};
		if (!AccumulateKeyState(input.right_key, 0, +1.0f))
			return {};
		if (!AccumulateKeyState(input.down_key,  1, -1.0f))
			return {};
		if (!AccumulateKeyState(input.up_key,    1, +1.0f))
			return {};
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

	std::optional<KeyState> InputDeviceInterface::DoGetInputState(Key input) const
	{
		return {};
	}

	std::optional<Input1DState> InputDeviceInterface::DoGetInputState(Input1D input) const
	{
		return {};
	}

	std::optional<Input2DState> InputDeviceInterface::DoGetInputState(Input2D input) const
	{	
		return {};
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
		if (std::optional<KeyState> input_state = GetInputState(input))
			return input_state->GetValue();
		return false;
	}

	float InputDeviceInterface::GetInputValue(Input1D input) const
	{
		if (std::optional<Input1DState> input_state = GetInputState(input))
			return input_state->GetValue();
		return 0.0f;
	}

	glm::vec2 InputDeviceInterface::GetInputValue(Input2D input) const
	{
		if (std::optional<Input2DState> input_state = GetInputState(input))
			return input_state->GetValue();
		return { 0.0f, 0.0f };
	}

	float InputDeviceInterface::GetInputValue(MappedInput1D input) const
	{
		if (std::optional<Input1DState> input_state = GetInputState(input))
			return input_state->GetValue();
		return 0.0f;
	}

	glm::vec2 InputDeviceInterface::GetInputValue(MappedInput2D input) const
	{
		if (std::optional<Input2DState> input_state = GetInputState(input))
			return input_state->GetValue();
		return { 0.0f, 0.0f };
	}

	InputStatus InputDeviceInterface::GetInputStatus(Key input) const
	{
		if (std::optional<KeyState> input_state = GetInputState(input))
			return input_state->GetStatus();
		return InputStatus::NONE;
	}

	InputStatus InputDeviceInterface::GetInputStatus(Input1D input) const
	{
		if (std::optional<Input1DState> input_state = GetInputState(input))
			return input_state->GetStatus();
		return InputStatus::NONE;
	}

	InputStatus InputDeviceInterface::GetInputStatus(Input2D input) const
	{
		if (std::optional<Input2DState> input_state = GetInputState(input))
			return input_state->GetStatus();
		return InputStatus::NONE;
	}

	InputStatus InputDeviceInterface::GetInputStatus(MappedInput1D input) const
	{
		if (std::optional<Input1DState> input_state = GetInputState(input))
			return input_state->GetStatus();
		return InputStatus::NONE;
	}

	InputStatus InputDeviceInterface::GetInputStatus(MappedInput2D input) const
	{
		if (std::optional<Input2DState> input_state = GetInputState(input))
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
			return IsInputJustActivated(state);
		});
	}

}; // namespace chaos
