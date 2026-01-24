#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_ENUM_BOOL_METHOD(InputStateResponseStatus, CHAOS_API);
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(InputStateQueryFlags, nullptr, CHAOS_API);
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(InputStateResponseFlags, nullptr, CHAOS_API);

	// requesting for an InputState is a two steps process
	// -first, we require authorization from InputConsumptionCache
	// -then,  we require the InputDevice for the state

	template<typename CONTAINER_TYPE, InputType INPUT_TYPE>
	InputStateResponseStatus DoTryConsumeInputHelper(INPUT_TYPE in_input, CONTAINER_TYPE& inout_consumed_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_query_flags, InputStateResponseFlags& out_response_flags)
	{
		// check wether the input is known
		if (in_input == INPUT_TYPE::UNKNOWN)
		{
			out_response_flags |= InputStateResponseFlags::UNKNOWN_INPUT;
			return InputStateResponseStatus::SUCCESS; // this is not an error
		}
	
		// input is not required yet
		auto it = inout_consumed_input.find(in_input);

		if (it == inout_consumed_input.end())
		{
			if (!HasAnyFlags(in_query_flags, InputStateQueryFlags::CONSULT_ONLY))
				inout_consumed_input.emplace(in_input, in_input_receiver);
			out_response_flags |= InputStateResponseFlags::SUCCESSFUL_INPUT | InputStateResponseFlags::KNOWN_INPUT;
			return InputStateResponseStatus::SUCCESS;
		}

		// the same input receiver requires for the input. that's ok
		if (it->second == in_input_receiver)
		{
			out_response_flags |= InputStateResponseFlags::SUCCESSFUL_INPUT | InputStateResponseFlags::KNOWN_INPUT;
			return InputStateResponseStatus::SUCCESS;
		}

		// another receiver requires the input. that's a rejection
		return InputStateResponseStatus::FAILURE;
	}

	InputStateResponseStatus InputConsumptionCache::DoTryConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_query_flags, InputStateResponseFlags& out_response_flags)
	{
		return DoTryConsumeInputHelper(in_input, consumed_keys, in_input_receiver, in_query_flags, out_response_flags);
	}

	InputStateResponseStatus InputConsumptionCache::DoTryConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_query_flags, InputStateResponseFlags& out_response_flags)
	{
		return DoTryConsumeInputHelper(in_input, consumed_input1D, in_input_receiver, in_query_flags, out_response_flags);
	}

	InputStateResponseStatus InputConsumptionCache::DoTryConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_query_flags, InputStateResponseFlags& out_response_flags)
	{
		return DoTryConsumeInputHelper(in_input, consumed_input2D, in_input_receiver, in_query_flags, out_response_flags);
	}

	template<InputType INPUT_TYPE>
	InputStateResponseStatus InputConsumptionCache::TryConsumeInputAndRelated(
		INPUT_TYPE in_input, 
		InputReceiverInterface const* in_input_receiver, 
		InputStateQueryFlags in_query_flags,
		InputStateResponseFlags& out_response_flags,
		LightweightFunction<InputStateResponseStatus()> consume_related_input_func
	)
	{
		// query for the input
		InputStateResponseStatus response_status = DoTryConsumeInput(in_input, in_input_receiver, in_query_flags, out_response_flags);

		// query for the related inputs
		InputStateResponseStatus intermediate_response_status = consume_related_input_func();
		if (intermediate_response_status == InputStateResponseStatus::FAILURE)
			return InputStateResponseStatus::FAILURE;
	
		// check whether all inputs are locked by another user
		// the order is important. 'consume_related_input_func' must have been called before, no matter what
		// -USER1 locks every inputs
		// -USER2 requests KEY1 -> this fails due to the lock
		// -USER1 requests KEY1 -> this fails due to USER2
		if (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver)
			return InputStateResponseStatus::FAILURE;

		return response_status;
	}

	InputStateResponseStatus InputConsumptionCache::TryConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_query_flags, InputStateResponseFlags& out_response_flags)
	{
		return TryConsumeInputAndRelated(in_input, in_input_receiver, in_query_flags, out_response_flags, [&]()
		{
			InputStateResponseStatus response_status = InputStateResponseStatus::SUCCESS;

			InputTools::EnumerateRelatedInputsDefinition(in_input, [&](VirtualKeyDefinition const& def)
			{
				response_status &= DoTryConsumeInput(def.input1D, in_input_receiver, in_query_flags, out_response_flags);
				return false; // don't stop and process next
			});
			return response_status;
		});
	}

	InputStateResponseStatus InputConsumptionCache::TryConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_query_flags, InputStateResponseFlags& out_response_flags)
	{
		return TryConsumeInputAndRelated(in_input, in_input_receiver, in_query_flags, out_response_flags, [&]()
		{
			InputStateResponseStatus response_status = InputStateResponseStatus::SUCCESS;

			InputTools::EnumerateRelatedInputsDefinition(in_input,
				[&](VirtualKeyDefinition const& def)
				{
					response_status &= DoTryConsumeInput(def.key, in_input_receiver, in_query_flags, out_response_flags);
					return false; // don't stop and process next
				},
				[&](VirtualInput2DDefinition const& def)
				{
					response_status &= DoTryConsumeInput(def.input2D, in_input_receiver, in_query_flags, out_response_flags);
					return false; // don't stop and process next			
				}
			);
			return response_status;
		});
	}

	InputStateResponseStatus InputConsumptionCache::TryConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_query_flags, InputStateResponseFlags& out_response_flags)
	{	
		return TryConsumeInputAndRelated(in_input, in_input_receiver, in_query_flags, out_response_flags, [&]()
		{
			InputStateResponseStatus response_status = InputStateResponseStatus::SUCCESS;

			InputTools::EnumerateRelatedInputsDefinition(in_input, [&](VirtualInput2DDefinition const& def)
			{
				response_status &= DoTryConsumeInput(def.input1D_x, in_input_receiver, in_query_flags, out_response_flags);
				response_status &= DoTryConsumeInput(def.input1D_y, in_input_receiver, in_query_flags, out_response_flags);
				return false; // don't stop and process next			
			});
			return response_status;
		});
	}

	InputStateResponse_t<MappedInput1D> InputConsumptionCache::QueryInputState(MappedInput1D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputStateQueryFlags in_query_flags)
	{
		InputStateResponseFlags  response_flags  = InputStateResponseFlags::NONE;
		InputStateResponseStatus response_status = InputStateResponseStatus::SUCCESS;
		for (Key key : {in_input.neg_key, in_input.pos_key})
			response_status &= TryConsumeInput(key, in_input_receiver, in_query_flags, response_flags);
		return FinalizeResponse(in_input, in_input_device, response_flags, response_status);
	}

	InputStateResponse_t<MappedInput2D> InputConsumptionCache::QueryInputState(MappedInput2D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputStateQueryFlags in_query_flags)
	{
		InputStateResponseFlags  response_flags  = InputStateResponseFlags::NONE;
		InputStateResponseStatus response_status = InputStateResponseStatus::SUCCESS;
		for (Key key : {in_input.left_key, in_input.right_key, in_input.down_key, in_input.up_key})
			response_status &= TryConsumeInput(key, in_input_receiver, in_query_flags, response_flags);
		return FinalizeResponse(in_input, in_input_device, response_flags, response_status);
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
