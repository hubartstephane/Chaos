#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool InputDeviceInterface::EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const
	{
		return func(this);
	}

	template<BasicInputType BASIC_INPUT_TYPE> 
	std::optional<InputState_t<BASIC_INPUT_TYPE>> InputDeviceInterface::GetInputStateHelper(BASIC_INPUT_TYPE input) const
	{
		if (input == BASIC_INPUT_TYPE::Unknown)
			return {};

		std::optional<InputState_t<BASIC_INPUT_TYPE>> result;
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

	std::optional<Input1DState> InputDeviceInterface::GetInputState(MappedInput1D const & input) const
	{
		std::optional<KeyState> neg_state = GetInputState(input.neg_key);
		std::optional<KeyState> pos_state = GetInputState(input.pos_key);

		if (!neg_state.has_value() && !pos_state.has_value())
			return {};

		Input1DState result;

		double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();		
		auto AccumulateKeyState = [&](std::optional<KeyState> const & in_state, float in_delta_value)
		{
			if (!in_state.has_value())
				return;

			if (in_state->value)
				result.value += in_delta_value;

			if (frame_time == in_state->update_time) // previous_value is valid
			{
				if (in_state->previous_value)
					result.previous_value += in_delta_value;
			}
			else // value is already an old value and can be used has previous frame value
			{
				if (in_state->value)
					result.previous_value += in_delta_value;
			}
		};

		AccumulateKeyState(neg_state, -1.0f);
		AccumulateKeyState(pos_state, +1.0f);
		result.update_time = frame_time; // value & previous_value computation are fresh from current frame

		return result;
	}

	std::optional<Input2DState> InputDeviceInterface::GetInputState(MappedInput2D const & input) const
	{
		std::optional<Input1DState> horizontal_state = GetInputState(MappedInput1D(input.left_key, input.right_key));
		std::optional<Input1DState> vertical_state = GetInputState(MappedInput1D(input.down_key, input.up_key));

		if (!horizontal_state.has_value() && !vertical_state.has_value())
			return {};

		Input2DState result;

		double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();
		auto AccumulateKeyState = [&](std::optional<Input1DState> const& in_state, size_t in_axis)
		{
			if (!in_state.has_value())
				return;

			result.value[in_axis] = in_state->value;

			if (frame_time == in_state->update_time) // previous_value is valid
			{
				result.previous_value[in_axis] = in_state->previous_value;
			}
			else // value is already an old value and can be used has previous frame value
			{
				result.previous_value[in_axis] = in_state->value;
			}
		};

		AccumulateKeyState(horizontal_state, 0);
		AccumulateKeyState(vertical_state, 1);
		result.update_time = frame_time; // value & previous_value are fresh from current frame

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
