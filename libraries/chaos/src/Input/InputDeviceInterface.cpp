#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(InputDeviceTypeMask, nullptr, CHAOS_API);

	bool InputDeviceInterface::EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const
	{
		return func(this);
	}

	template<BasicInputType BASIC_INPUT_TYPE> 
	InputDeviceQueryResult<InputTypeToValueType_t<BASIC_INPUT_TYPE>> InputDeviceInterface::GetInputStateHelper(BASIC_INPUT_TYPE input) const
	{
		if (input == BASIC_INPUT_TYPE::Unknown)
			return {};

		InputDeviceQueryResult<InputTypeToValueType_t<BASIC_INPUT_TYPE>> result;
		EnumerateDeviceHierarchy([this, &result, input](InputDeviceInterface const * in_input_device)
		{
			result = in_input_device->DoGetInputState(input);
			return result.state.has_value(); // continue until some result is found
		});
		return result;
	}

	InputDeviceQueryResult<bool> InputDeviceInterface::GetInputState(Key input) const
	{
		return GetInputStateHelper(input);
	}

	InputDeviceQueryResult<float> InputDeviceInterface::GetInputState(Input1D input) const
	{
		return GetInputStateHelper(input);
	}

	InputDeviceQueryResult<glm::vec2> InputDeviceInterface::GetInputState(Input2D input) const
	{	
		return GetInputStateHelper(input);
	}

	InputDeviceQueryResult<float> InputDeviceInterface::GetInputState(MappedInput1D const & input) const
	{
		InputDeviceQueryResult<bool> neg_input_result = GetInputState(input.neg_key);
		InputDeviceQueryResult<bool> pos_input_result = GetInputState(input.pos_key);

		if (!neg_input_result.state.has_value() && !pos_input_result.state.has_value())
			return {};

		Input1DState result;
		InputDeviceTypeMask involved_devices = InputDeviceTypeMask::None;

		double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();		
		auto AccumulateKeyState = [&](InputDeviceQueryResult<bool> const & in_state, float in_delta_value)
		{
			if (!in_state.state.has_value())
				return;

			if (in_state.state->IsActive())
			{
				result.value += in_delta_value;
				involved_devices |= in_state.involved_devices;
			}

			if (frame_time == in_state.state->update_time) // previous_value is valid
			{
				if (in_state.state->previous_value)
					result.previous_value += in_delta_value;
			}
			else // value is already an old value and can be used has previous frame value
			{
				if (in_state.state->value)
					result.previous_value += in_delta_value;
			}
		};

		AccumulateKeyState(neg_input_result, -1.0f);
		AccumulateKeyState(pos_input_result, +1.0f);
		result.update_time = frame_time; // value & previous_value computation are fresh from current frame

		return {result, (result.IsActive()? involved_devices : InputDeviceTypeMask::None) }; // if both POS & NEG direction are pressed altogether, set involved devices to none
	}

	InputDeviceQueryResult<glm::vec2> InputDeviceInterface::GetInputState(MappedInput2D const & input) const
	{
		InputDeviceQueryResult<float> horizontal_input_result = GetInputState(MappedInput1D(input.left_key, input.right_key));
		InputDeviceQueryResult<float> vertical_input_result = GetInputState(MappedInput1D(input.down_key, input.up_key));

		if (!horizontal_input_result.state.has_value() && !vertical_input_result.state.has_value())
			return {};

		Input2DState result;
		InputDeviceTypeMask involved_devices = InputDeviceTypeMask::None;

		double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();
		auto AccumulateKeyState = [&](InputDeviceQueryResult<float> const& in_state, size_t in_axis)
		{
			if (!in_state.state.has_value())
				return;

			result.value[in_axis] = in_state.state->value;
			if (in_state.state->IsActive())
				involved_devices |= in_state.involved_devices;

			if (frame_time == in_state.state->update_time) // previous_value is valid
			{
				result.previous_value[in_axis] = in_state.state->previous_value;
			}
			else // value is already an old value and can be used has previous frame value
			{
				result.previous_value[in_axis] = in_state.state->value;
			}
		};

		AccumulateKeyState(horizontal_input_result, 0);
		AccumulateKeyState(vertical_input_result, 1);
		result.update_time = frame_time; // value & previous_value are fresh from current frame

		return { result, (result.IsActive() ? involved_devices : InputDeviceTypeMask::None) };
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

	InputDeviceQueryResult<bool> InputDeviceInterface::DoGetInputState(Key input) const
	{
		return {};
	}

	InputDeviceQueryResult<float> InputDeviceInterface::DoGetInputState(Input1D input) const
	{
		return {};
	}

	InputDeviceQueryResult<glm::vec2> InputDeviceInterface::DoGetInputState(Input2D input) const
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
