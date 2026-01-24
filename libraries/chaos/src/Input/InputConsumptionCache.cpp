#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(InputStateQueryFlags, nullptr, CHAOS_API);
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(InputStateResponseFlags, nullptr, CHAOS_API);

	// requesting for an InputState is a two steps process
	// -first, we require authorization from InputConsumptionCache
	// -then,  we require the InputDevice for the state
	//
	// if, during first step their is a rejection, we don't bother asking InputDevice at all
	// so we don't get extra information as UNHANDLED_INPUT
	// that's why, for consistency, if we get a rejection, we just return REJECTED_INPUT state

	template<typename CONTAINER_TYPE, InputType INPUT_TYPE>
	InputStateResponseFlags DoTryConsumeInputHelper(INPUT_TYPE in_input, CONTAINER_TYPE& inout_consumed_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags)
	{
		// check wether the input is known
		if (in_input == INPUT_TYPE::UNKNOWN)
			return InputStateResponseFlags::UNKNOWN_INPUT;
	
		// input is not required yet
		auto it = inout_consumed_input.find(in_input);

		if (it == inout_consumed_input.end())
		{
			if (!HasAnyFlags(in_flags, InputStateQueryFlags::CONSULT_ONLY))
				inout_consumed_input.emplace(in_input, in_input_receiver);
			return InputStateResponseFlags::SUCCESSFUL_INPUT | InputStateResponseFlags::KNOWN_INPUT;
		}

		// the same input receiver requires for the input. that's ok
		if (it->second == in_input_receiver)
			return InputStateResponseFlags::SUCCESSFUL_INPUT | InputStateResponseFlags::KNOWN_INPUT;

		// another receiver requires the input. that's a rejection
		return InputStateResponseFlags::REJECTED_INPUT;
	}

	InputStateResponseFlags InputConsumptionCache::DoTryConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags)
	{
		return DoTryConsumeInputHelper(in_input, consumed_keys, in_input_receiver, in_flags);
	}

	InputStateResponseFlags InputConsumptionCache::DoTryConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags)
	{
		return DoTryConsumeInputHelper(in_input, consumed_input1D, in_input_receiver, in_flags);
	}

	InputStateResponseFlags InputConsumptionCache::DoTryConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags)
	{
		return DoTryConsumeInputHelper(in_input, consumed_input2D, in_input_receiver, in_flags);
	}

	template<InputType INPUT_TYPE>
	InputStateResponseFlags InputConsumptionCache::TryConsumeInputAndRelated(
		INPUT_TYPE in_input, 
		InputReceiverInterface const* in_input_receiver, 
		InputStateQueryFlags in_flags, 
		LightweightFunction<InputStateResponseFlags(InputStateResponseFlags)> consume_related_input_func
	)
	{
		// query for the input
		InputStateResponseFlags result = DoTryConsumeInput(in_input, in_input_receiver, in_flags);

		// query for the related inputs
		result = consume_related_input_func(result);
	
		// check whether all inputs are locked by another user
		// the order is important. 'consume_related_input_func' must have been called before, no matter what
		// -USER1 locks every inputs
		// -USER2 requests KEY1 -> this fails due to the lock
		// -USER1 requests KEY1 -> this fails due to USER2
		if (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver)
			return InputStateResponseFlags::REJECTED_INPUT;

		// In case of rejection, we only keep rejection bit
		if (HasAnyFlags(result, InputStateResponseFlags::REJECTED_INPUT))
			return InputStateResponseFlags::REJECTED_INPUT;

		return result;
	}

	InputStateResponseFlags InputConsumptionCache::TryConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags)
	{
		return TryConsumeInputAndRelated(in_input, in_input_receiver, in_flags, [&](InputStateResponseFlags result)
		{
			InputTools::EnumerateRelatedInputsDefinition(in_input, [&](VirtualKeyDefinition const& def)
			{
				result |= DoTryConsumeInput(def.input1D, in_input_receiver, in_flags);
				return false; // don't stop and process next
			});
			return result;
		});
	}

	InputStateResponseFlags InputConsumptionCache::TryConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags)
	{
		return TryConsumeInputAndRelated(in_input, in_input_receiver, in_flags, [&](InputStateResponseFlags result)
		{
			InputTools::EnumerateRelatedInputsDefinition(in_input,
				[&](VirtualKeyDefinition const& def)
				{
					result |= DoTryConsumeInput(def.key, in_input_receiver, in_flags);
					return false; // don't stop and process next
				},
				[&](VirtualInput2DDefinition const& def)
				{
					result |= DoTryConsumeInput(def.input2D, in_input_receiver, in_flags);
					return false; // don't stop and process next			
				}
			);
			return result;
		});
	}

	InputStateResponseFlags InputConsumptionCache::TryConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags)
	{
		return TryConsumeInputAndRelated(in_input, in_input_receiver, in_flags, [&](InputStateResponseFlags result)
		{
			InputTools::EnumerateRelatedInputsDefinition(in_input, [&](VirtualInput2DDefinition const& def)
			{
				result |= DoTryConsumeInput(def.input1D_x, in_input_receiver, in_flags);
				result |= DoTryConsumeInput(def.input1D_y, in_input_receiver, in_flags);
				return false; // don't stop and process next			
			});
			return result;
		});
	}

	InputStateResponse_t<MappedInput1D> InputConsumptionCache::QueryInputState(MappedInput1D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputStateQueryFlags in_flags)
	{
		InputStateResponseFlags result_flags = InputStateResponseFlags::NONE;
		for (Key key : {in_input.neg_key, in_input.pos_key})
			result_flags |= TryConsumeInput(key, in_input_receiver, in_flags);
		return InputStateResponseFinalization(in_input, in_input_device, result_flags);
	}

	InputStateResponse_t<MappedInput2D> InputConsumptionCache::QueryInputState(MappedInput2D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputStateQueryFlags in_flags)
	{
		InputStateResponseFlags result_flags = InputStateResponseFlags::NONE;
		for (Key key : {in_input.left_key, in_input.right_key, in_input.down_key, in_input.up_key})
			result_flags |= TryConsumeInput(key, in_input_receiver, in_flags);
		return InputStateResponseFinalization(in_input, in_input_device, result_flags);
	}

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

}; // namespace chaos
