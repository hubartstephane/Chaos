#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
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

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<MappedInput1D> in_input)
	{
		// a single rejection among input and related inputs is enough for a whole rejection
		// (but we still need to lock all related inputs even if the very first is a rejection)
		bool result = true;

		MappedInput1D const & effective_input = GetInput(in_input);
		TaggedInputFlags input_flags = GetInputFlags(in_input);

		result &= TryConsumeInput(in_input_receiver, in_input_device, TaggedInput(effective_input.neg_key, input_flags)); // propagate all flags from TaggedInput to basic input
		result &= TryConsumeInput(in_input_receiver, in_input_device, TaggedInput(effective_input.pos_key, input_flags));

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ? // somebody else has locked all inputs
			false :
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<MappedInput2D> in_input)
	{
		// a single rejection among input and related inputs is enough for a whole rejection
		// (but we still need to lock all related inputs even if the very first is a rejection)
		bool result = true;

		MappedInput2D const& effective_input = GetInput(in_input);
		TaggedInputFlags input_flags = GetInputFlags(in_input);

		result &= TryConsumeInput(in_input_receiver, in_input_device, TaggedInput(effective_input.left_key, input_flags)); // propagate all flags from TaggedInput to basic input
		result &= TryConsumeInput(in_input_receiver, in_input_device, TaggedInput(effective_input.right_key, input_flags));
		result &= TryConsumeInput(in_input_receiver, in_input_device, TaggedInput(effective_input.down_key, input_flags));
		result &= TryConsumeInput(in_input_receiver, in_input_device, TaggedInput(effective_input.up_key, input_flags));

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ? // somebody else has locked all inputs
			false :
			result;
	}


	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Key> in_input)
	{
		// a single rejection among input and related inputs is enough for a whole rejection
		// (but we still need to lock all related inputs even if the very first is a rejection)
		bool result = true;

		// handle key
		result &= DoTryConsumeInput(in_input_receiver, in_input_device, in_input);
		// handle 'virtual related' keys/inputs
		InputTools::EnumerateRelatedInputs(in_input, [&](Key key, Input1D input)
		{
			result &= DoTryConsumeInput(in_input_receiver, in_input_device, input);
			return false; // don't stop and process next
		});

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ? // somebody else has locked all inputs
			false:
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Input1D> in_input)
	{
		// a single rejection among input and related inputs is enough for a whole rejection
		// (but we still need to lock all related inputs even if the very first is a rejection)
		bool result = true;

		// handle axis
		result &= DoTryConsumeInput(in_input_receiver, in_input_device, in_input);
		// handle 'virtual related' keys/inputs
		InputTools::EnumerateRelatedInputs(in_input, [&](Key key, Input1D input)
		{
			result &= DoTryConsumeInput(in_input_receiver, in_input_device, key);
			return false; // don't stop and process next
		},
		[&](Input2D input2D, Input1D input1D_x, Input1D input1D_y)
		{
			result &= DoTryConsumeInput(in_input_receiver, in_input_device, input2D);
			return false; // don't stop and process next
		});

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ? // somebody else has locked all inputs
			false :
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Input2D> in_input)
	{
		// a single rejection among input and related inputs is enough for a whole rejection
		// (but we still need to lock all related inputs even if the very first is a rejection)
		bool result = true;

		// handle stick
		result &= DoTryConsumeInput(in_input_receiver, in_input_device, in_input);

		// handle 'virtual related' inputs
		InputTools::EnumerateRelatedInputs(in_input, [&](Input2D input2D, Input1D input1D_x, Input1D input1D_y)
		{
			result &= DoTryConsumeInput(in_input_receiver, in_input_device, input1D_x);
			result &= DoTryConsumeInput(in_input_receiver, in_input_device, input1D_y);
			return false; // don't stop and process next
		});

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ? // somebody else has locked all inputs
			false :
			result;
	}

	template<typename CONTAINER_TYPE, InputType INPUT_TYPE>
	bool TryInsertConsumedInput(CONTAINER_TYPE & inout_consumed_input, InputReceiverInterface const* in_input_receiver, InputState_t<INPUT_TYPE> const* in_state, TaggedInput<INPUT_TYPE> in_input)
	{
		auto consumed_input_key = std::make_pair(in_input, in_state);

		auto it = inout_consumed_input.find(consumed_input_key);

		// input is not required yet
		if (it == inout_consumed_input.end())
		{
			if (!HasAnyFlags(in_input.flags, TaggedInputFlags::CONSULT_ONLY))
				inout_consumed_input.emplace(consumed_input_key, in_input_receiver);
			return true;
		}
		// the same input receiver requires for the input. that's ok
		if (it->second == in_input_receiver)
			return true;
		// another receiver requires the input. that's a rejection
		return false;
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Key> in_input)
	{
		auto const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return true; // if the device doesn't handle this input, this shouldn't be considered as a rejection (for example if in_input == UNKOWN that is legit)

		return TryInsertConsumedInput(consumed_keys, in_input_receiver, state, in_input);
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Input1D> in_input)
	{
		auto const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return true; // if the device doesn't handle this input, this shouldn't be considered as a rejection (for example if in_input == UNKOWN that is legit)

		return TryInsertConsumedInput(consumed_input1D, in_input_receiver, state, in_input);
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, TaggedInput<Input2D> in_input)
	{
		auto const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return true; // if the device doesn't handle this input, this shouldn't be considered as a rejection (for example if in_input == UNKOWN that is legit)

		return TryInsertConsumedInput(consumed_input2D, in_input_receiver, state, in_input);
	}

}; // namespace chaos
