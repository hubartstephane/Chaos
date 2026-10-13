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

	template<BasicInputType BASIC_INPUT_TYPE>
	InputStateResponseStatus InputConsumptionCache::DoTryConsumeInput(
		BASIC_INPUT_TYPE in_input, 
		InputReceiverInterface const* in_input_receiver, 
		InputStateQueryFlags in_query_flags, 
		InputStateResponseFlags& out_response_flags)
	{
		// check whether someone queried for all inputs
		if constexpr (std::is_same_v<Key, BASIC_INPUT_TYPE>)
			if (all_key_consumer.has_value() && all_key_consumer.value() != in_input_receiver)
				return InputStateResponseStatus::Failure;
		if constexpr (std::is_same_v<Input1D, BASIC_INPUT_TYPE>)
			if (all_input1D_consumer.has_value() && all_input1D_consumer.value() != in_input_receiver)
				return InputStateResponseStatus::Failure;
		if constexpr (std::is_same_v<Input2D, BASIC_INPUT_TYPE>)
			if (all_input2D_consumer.has_value() && all_input2D_consumer.value() != in_input_receiver)
				return InputStateResponseStatus::Failure;

		// check wether the input is known
		if (in_input == BASIC_INPUT_TYPE::Unknown)
		{
			out_response_flags |= InputStateResponseFlags::UnknownInput;
			return InputStateResponseStatus::Success; // this is not an error
		}

		auto consume_input_func = [&](auto & inout_consumed_input)
		{
			// input is not required yet
			auto it = inout_consumed_input.find(in_input);

			if (it == inout_consumed_input.end())
			{
				if (!HasAnyFlags(in_query_flags, InputStateQueryFlags::ConsultOnly))
					inout_consumed_input.emplace(in_input, in_input_receiver);
				out_response_flags |= InputStateResponseFlags::SuccessfulInput | InputStateResponseFlags::KnownInput;
				return InputStateResponseStatus::Success;
			}

			// the same input receiver requires for the input. that's ok
			if (it->second == in_input_receiver)
			{
				out_response_flags |= InputStateResponseFlags::SuccessfulInput | InputStateResponseFlags::KnownInput;
				return InputStateResponseStatus::Success;
			}

			// another receiver requires the input. that's a rejection
			return InputStateResponseStatus::Failure;
		};

		// dispatch request to appropriate input map
		if constexpr (std::is_same_v<Key, BASIC_INPUT_TYPE>)
			return consume_input_func(consumed_keys);
		if constexpr (std::is_same_v<Input1D, BASIC_INPUT_TYPE>)
			return consume_input_func(consumed_input1D);
		if constexpr (std::is_same_v<Input2D, BASIC_INPUT_TYPE>)
			return consume_input_func(consumed_input2D);

		// should never happen
		assert(0);
		return InputStateResponseStatus::Failure;
	}

	template<BasicInputType BASIC_INPUT_TYPE>
	InputStateResponseStatus InputConsumptionCache::TryConsumeInputAndRelated(
		BASIC_INPUT_TYPE in_input, 
		InputReceiverInterface const* in_input_receiver, 
		InputStateQueryFlags in_query_flags,
		InputStateResponseFlags& out_response_flags,
		ConsumeRelatedInputFunc consume_related_input_func
	)
	{
		InputStateResponseStatus response_status = InputStateResponseStatus::Success;
		response_status &= DoTryConsumeInput(in_input, in_input_receiver, in_query_flags, out_response_flags);
		response_status &= consume_related_input_func(in_input_receiver, in_query_flags, out_response_flags);
		return response_status;
	}

	InputStateResponseStatus InputConsumptionCache::TryConsumeInput(
		Key in_input, 
		InputReceiverInterface const* in_input_receiver, 
		InputStateQueryFlags in_query_flags, 
		InputStateResponseFlags& out_response_flags)
	{
		auto consume_related_func = [this, in_input](InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_query_flags, InputStateResponseFlags& out_response_flags)
		{
			InputStateResponseStatus response_status = InputStateResponseStatus::Success;

			InputTools::EnumerateRelatedInputsDefinition(in_input, [&](VirtualKeyDefinition const& def)
			{
				response_status &= DoTryConsumeInput(def.input1D, in_input_receiver, in_query_flags, out_response_flags);
				return false; // don't stop and process next
			});
			return response_status;
		};
		return TryConsumeInputAndRelated(in_input, in_input_receiver, in_query_flags, out_response_flags, consume_related_func);
	}

	InputStateResponseStatus InputConsumptionCache::TryConsumeInput(
		Input1D in_input, 
		InputReceiverInterface const* in_input_receiver, 
		InputStateQueryFlags in_query_flags, 
		InputStateResponseFlags& out_response_flags)
	{
		auto consume_related_func = [this, in_input](InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_query_flags, InputStateResponseFlags& out_response_flags)
		{
			InputStateResponseStatus response_status = InputStateResponseStatus::Success;

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
		};
		return TryConsumeInputAndRelated(in_input, in_input_receiver, in_query_flags, out_response_flags, consume_related_func);
	}

	InputStateResponseStatus InputConsumptionCache::TryConsumeInput(
		Input2D in_input, 
		InputReceiverInterface const* in_input_receiver, 
		InputStateQueryFlags in_query_flags, 
		InputStateResponseFlags& out_response_flags)
	{	
		auto consume_related_func = [this, in_input](InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_query_flags, InputStateResponseFlags& out_response_flags)
		{
			InputStateResponseStatus response_status = InputStateResponseStatus::Success;

			InputTools::EnumerateRelatedInputsDefinition(in_input, [&](VirtualInput2DDefinition const& def)
			{
				response_status &= DoTryConsumeInput(def.input1D_x, in_input_receiver, in_query_flags, out_response_flags);
				response_status &= DoTryConsumeInput(def.input1D_y, in_input_receiver, in_query_flags, out_response_flags);
				return false; // don't stop and process next			
			});
			return response_status;
		};
		return TryConsumeInputAndRelated(in_input, in_input_receiver, in_query_flags, out_response_flags, consume_related_func);
	}



















	InputStateResponse_t<MappedInput1D> InputConsumptionCache::QueryInputState(MappedInput1D const &in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputStateQueryFlags in_query_flags)
	{
		std::initializer_list<Key> keys = { in_input.neg_key, in_input.pos_key };
		return QueryMappedInputState(in_input, keys, in_input_receiver, in_input_device, in_query_flags);
	}

	InputStateResponse_t<MappedInput2D> InputConsumptionCache::QueryInputState(MappedInput2D const &in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputStateQueryFlags in_query_flags)
	{
		std::initializer_list<Key> keys = { in_input.left_key, in_input.right_key, in_input.down_key, in_input.up_key };
		return QueryMappedInputState(in_input, keys, in_input_receiver, in_input_device, in_query_flags);
	}

	bool InputConsumptionCache::SetAllKeyConsumer(InputReceiverInterface const* in_input_receiver)
	{
		if (!all_key_consumer.has_value() || all_key_consumer.value() == in_input_receiver)
		{
			all_key_consumer = in_input_receiver;
			return true;
		}
		return false;
	}

	bool InputConsumptionCache::SetAllInput1DConsumer(InputReceiverInterface const* in_input_receiver)
	{
		if (!all_input1D_consumer.has_value() || all_input1D_consumer.value() == in_input_receiver)
		{
			all_input1D_consumer = in_input_receiver;
			return true;
		}
		return false;
	}

	bool InputConsumptionCache::SetAllInput2DConsumer(InputReceiverInterface const* in_input_receiver)
	{
		if (!all_input2D_consumer.has_value() || all_input2D_consumer.value() == in_input_receiver)
		{
			all_input2D_consumer = in_input_receiver;
			return true;
		}
		return false;
	}

	bool InputConsumptionCache::SetAllInputConsumer(InputReceiverInterface const* in_input_receiver)
	{
		// we must force all 3 calls to be done
		bool result = false;
		result |= SetAllKeyConsumer(in_input_receiver);
		result |= SetAllInput1DConsumer(in_input_receiver);
		result |= SetAllInput2DConsumer(in_input_receiver);
		return result;
	}

	void InputConsumptionCache::Clear()
	{
		consumed_keys.clear();
		consumed_input1D.clear();
		consumed_input2D.clear();

		all_key_consumer.reset();
		all_input1D_consumer.reset();
		all_input2D_consumer.reset();
	}

}; // namespace chaos
