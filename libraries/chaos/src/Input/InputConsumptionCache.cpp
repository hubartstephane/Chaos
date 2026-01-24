#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(QueryInputStateFlags, nullptr, CHAOS_API);
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(QueryInputStateResultFlags, nullptr, CHAOS_API);

	// requesting for an InputState is a two steps process
	// -first, we require authorization from InputConsumptionCache
	// -then,  we require the InputDevice for the state
	//
	// if, during first step their is a rejection, we don't bother asking InputDevice at all
	// so we don't get extra information as UNHANDLED_INPUT
	// that's why, for consistency, if we get a rejection, we just return REJECTED_INPUT state

	template<typename CONTAINER_TYPE, InputType INPUT_TYPE>
	QueryInputStateResultFlags DoCheckAndConsumeInputHelper(INPUT_TYPE in_input, CONTAINER_TYPE& inout_consumed_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags)
	{
		// check wether the input is known
		if (in_input == INPUT_TYPE::UNKNOWN)
			return QueryInputStateResultFlags::UNKNOWN_INPUT;
	
		// input is not required yet
		auto it = inout_consumed_input.find(in_input);

		if (it == inout_consumed_input.end())
		{
			if (!HasAnyFlags(in_flags, QueryInputStateFlags::CONSULT_ONLY))
				inout_consumed_input.emplace(in_input, in_input_receiver);
			return QueryInputStateResultFlags::SUCCESSFULL_INPUT | QueryInputStateResultFlags::KNOWN_INPUT;
		}

		// the same input receiver requires for the input. that's ok
		if (it->second == in_input_receiver)
			return QueryInputStateResultFlags::SUCCESSFULL_INPUT | QueryInputStateResultFlags::KNOWN_INPUT;

		// another receiver requires the input. that's a rejection
		return QueryInputStateResultFlags::REJECTED_INPUT;
	}

	QueryInputStateResultFlags InputConsumptionCache::DoCheckAndConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags)
	{
		return DoCheckAndConsumeInputHelper(in_input, consumed_keys, in_input_receiver, in_flags);
	}

	QueryInputStateResultFlags InputConsumptionCache::DoCheckAndConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags)
	{
		return DoCheckAndConsumeInputHelper(in_input, consumed_input1D, in_input_receiver, in_flags);
	}

	QueryInputStateResultFlags InputConsumptionCache::DoCheckAndConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags)
	{
		return DoCheckAndConsumeInputHelper(in_input, consumed_input2D, in_input_receiver, in_flags);
	}

	template<InputType INPUT_TYPE>
	QueryInputStateResultFlags InputConsumptionCache::DoCheckAndConsumeInputAndRelated(
		INPUT_TYPE in_input, 
		InputReceiverInterface const* in_input_receiver, 
		QueryInputStateFlags in_flags, 
		LightweightFunction<QueryInputStateResultFlags(QueryInputStateResultFlags)> check_related_input_func
	)
	{
		// query for the input
		QueryInputStateResultFlags result = DoCheckAndConsumeInput(in_input, in_input_receiver, in_flags);

		// query for the related inputs
		result = check_related_input_func(result);
	
		// all inputs are locked ?
		if (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver)
			return QueryInputStateResultFlags::REJECTED_INPUT;

		// In case of rejection, we only keep rejection bit
		if (HasAnyFlags(result, QueryInputStateResultFlags::REJECTED_INPUT))
			return QueryInputStateResultFlags::REJECTED_INPUT;

		return result;
	}

	QueryInputStateResultFlags InputConsumptionCache::CheckAndConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags)
	{
		return DoCheckAndConsumeInputAndRelated(in_input, in_input_receiver, in_flags, [&](QueryInputStateResultFlags result)
		{
			InputTools::EnumerateRelatedInputsDefinition(in_input, [&](VirtualKeyDefinition const& def)
			{
				result |= DoCheckAndConsumeInput(def.input1D, in_input_receiver, in_flags);
				return false; // don't stop and process next
			});
			return result;
		});
	}

	QueryInputStateResultFlags InputConsumptionCache::CheckAndConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags)
	{
		return DoCheckAndConsumeInputAndRelated(in_input, in_input_receiver, in_flags, [&](QueryInputStateResultFlags result)
		{
			InputTools::EnumerateRelatedInputsDefinition(in_input,
				[&](VirtualKeyDefinition const& def)
				{
					result |= DoCheckAndConsumeInput(def.key, in_input_receiver, in_flags);
					return false; // don't stop and process next
				},
				[&](VirtualInput2DDefinition const& def)
				{
					result |= DoCheckAndConsumeInput(def.input2D, in_input_receiver, in_flags);
					return false; // don't stop and process next			
				}
			);
			return result;
		});
	}

	QueryInputStateResultFlags InputConsumptionCache::CheckAndConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags)
	{
		return DoCheckAndConsumeInputAndRelated(in_input, in_input_receiver, in_flags, [&](QueryInputStateResultFlags result)
		{
			InputTools::EnumerateRelatedInputsDefinition(in_input, [&](VirtualInput2DDefinition const& def)
			{
				result |= DoCheckAndConsumeInput(def.input1D_x, in_input_receiver, in_flags);
				result |= DoCheckAndConsumeInput(def.input1D_y, in_input_receiver, in_flags);
				return false; // don't stop and process next			
			});
			return result;
		});
	}

	QueryInputStateResult_t<MappedInput1D> InputConsumptionCache::QueryInputState(MappedInput1D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, QueryInputStateFlags in_flags)
	{
		QueryInputStateResultFlags result_flags = QueryInputStateResultFlags::NONE;
		for (Key key : {in_input.neg_key, in_input.pos_key})
			result_flags |= CheckAndConsumeInput(key, in_input_receiver, in_flags);
		return QueryInputStateResultFinalization(in_input, in_input_device, result_flags);
	}

	QueryInputStateResult_t<MappedInput2D> InputConsumptionCache::QueryInputState(MappedInput2D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, QueryInputStateFlags in_flags)
	{
		QueryInputStateResultFlags result_flags = QueryInputStateResultFlags::NONE;
		for (Key key : {in_input.left_key, in_input.right_key, in_input.down_key, in_input.up_key})
			result_flags |= CheckAndConsumeInput(key, in_input_receiver, in_flags);
		return QueryInputStateResultFinalization(in_input, in_input_device, result_flags);
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
