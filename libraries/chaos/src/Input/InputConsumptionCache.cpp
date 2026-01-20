#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(InputConsumptionFlags, nullptr, CHAOS_API);

	void InputConsumptionCache::SetConsumeAllInputs(InputReceiverInterface const* in_input_receiver)
	{
		if (!all_inputs_consumer.has_value())
			all_inputs_consumer = in_input_receiver;
	}

	void InputConsumptionCache::Clear()
	{
		consumed_keys.clear();
		consumed_input1D.clear();
		consumed_input2D.clear();
		all_inputs_consumer.reset();
	}

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, MappedInput1D in_input, InputConsumptionFlags in_flags)
	{
		// a single rejection among input and related inputs is enough for a whole rejection
		// (but we still need to lock all related inputs even if the very first is a rejection)
		bool result = true;

		result &= TryConsumeInput(in_input_receiver, in_input_device, in_input.neg_key, in_flags);
		result &= TryConsumeInput(in_input_receiver, in_input_device, in_input.pos_key, in_flags);

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ? // somebody else has locked all inputs
			false :
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, MappedInput2D in_input, InputConsumptionFlags in_flags)
	{
		// a single rejection among input and related inputs is enough for a whole rejection
		// (but we still need to lock all related inputs even if the very first is a rejection)
		bool result = true;

		result &= TryConsumeInput(in_input_receiver, in_input_device, in_input.left_key, in_flags);
		result &= TryConsumeInput(in_input_receiver, in_input_device, in_input.right_key, in_flags);
		result &= TryConsumeInput(in_input_receiver, in_input_device, in_input.down_key, in_flags);
		result &= TryConsumeInput(in_input_receiver, in_input_device, in_input.up_key, in_flags);

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ? // somebody else has locked all inputs
			false :
			result;
	}


	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, Key in_input, InputConsumptionFlags in_flags)
	{
		// a single rejection among input and related inputs is enough for a whole rejection
		// (but we still need to lock all related inputs even if the very first is a rejection)
		bool result = true;

		// handle key
		result &= DoTryConsumeInput(in_input_receiver, in_input_device, in_input, in_flags);
		// handle 'virtual related' keys/inputs
		InputTools::EnumerateRelatedInputs(in_input, [&](Key key, Input1D input)
		{
			result &= DoTryConsumeInput(in_input_receiver, in_input_device, input, in_flags);
			return false; // don't stop and process next
		});

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ? // somebody else has locked all inputs
			false:
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, Input1D in_input, InputConsumptionFlags in_flags)
	{
		// a single rejection among input and related inputs is enough for a whole rejection
		// (but we still need to lock all related inputs even if the very first is a rejection)
		bool result = true;

		// handle axis
		result &= DoTryConsumeInput(in_input_receiver, in_input_device, in_input, in_flags);
		// handle 'virtual related' keys/inputs
		InputTools::EnumerateRelatedInputs(in_input, [&](Key key, Input1D input)
		{
			result &= DoTryConsumeInput(in_input_receiver, in_input_device, key, in_flags);
			return false; // don't stop and process next
		},
		[&](Input2D input2D, Input1D input1D_x, Input1D input1D_y)
		{
			result &= DoTryConsumeInput(in_input_receiver, in_input_device, input2D, in_flags);
			return false; // don't stop and process next
		});

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ? // somebody else has locked all inputs
			false :
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, Input2D in_input, InputConsumptionFlags in_flags)
	{
		// a single rejection among input and related inputs is enough for a whole rejection
		// (but we still need to lock all related inputs even if the very first is a rejection)
		bool result = true;

		// handle stick
		result &= DoTryConsumeInput(in_input_receiver, in_input_device, in_input, in_flags);

		// handle 'virtual related' inputs
		InputTools::EnumerateRelatedInputs(in_input, [&](Input2D input2D, Input1D input1D_x, Input1D input1D_y)
		{
			result &= DoTryConsumeInput(in_input_receiver, in_input_device, input1D_x, in_flags);
			result &= DoTryConsumeInput(in_input_receiver, in_input_device, input1D_y, in_flags);
			return false; // don't stop and process next
		});

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ? // somebody else has locked all inputs
			false :
			result;
	}

	template<typename CONTAINER_TYPE, InputType INPUT_TYPE>
	bool TryInsertConsumedInput(CONTAINER_TYPE & inout_consumed_input, InputReceiverInterface const* in_input_receiver, INPUT_TYPE in_input, InputConsumptionFlags in_flags)
	{
		auto it = inout_consumed_input.find(in_input);

		// input is not required yet
		if (it == inout_consumed_input.end())
		{
			if (!HasAnyFlags(in_flags, InputConsumptionFlags::CONSULT_ONLY))
				inout_consumed_input.emplace(in_input, in_input_receiver);
			return true;
		}
		// the same input receiver requires for the input. that's ok
		if (it->second == in_input_receiver)
			return true;
		// another receiver requires the input. that's a rejection
		return false;
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, Key in_input, InputConsumptionFlags in_flags)
	{
		std::optional<KeyState> state = in_input_device->GetInputState(in_input);
		if (!state.has_value())
			return true; // if the device doesn't handle this input, this shouldn't be considered as a rejection (for example if in_input == UNKOWN that is legit)

		return TryInsertConsumedInput(consumed_keys, in_input_receiver, in_input, in_flags);
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, Input1D in_input, InputConsumptionFlags in_flags)
	{
		std::optional<Input1DState> state = in_input_device->GetInputState(in_input);
		if (!state.has_value())
			return true; // if the device doesn't handle this input, this shouldn't be considered as a rejection (for example if in_input == UNKOWN that is legit)

		return TryInsertConsumedInput(consumed_input1D, in_input_receiver, in_input, in_flags);
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, Input2D in_input, InputConsumptionFlags in_flags)
	{
		std::optional<Input2DState> state = in_input_device->GetInputState(in_input);
		if (!state.has_value())
			return true; // if the device doesn't handle this input, this shouldn't be considered as a rejection (for example if in_input == UNKOWN that is legit)

		return TryInsertConsumedInput(consumed_input2D, in_input_receiver, in_input, in_flags);
	}

}; // namespace chaos
