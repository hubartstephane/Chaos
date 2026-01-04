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


	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, Key const& in_input, InputDeviceInterface const* in_input_device, TaggedInputFlags in_flags) // check whether the key is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle key
		result &= DoTryConsumeInput(in_input_receiver, in_input, in_input_device, in_flags);
		// handle 'virtual related' keys/inputs
		InputTools::EnumerateRelatedInputs(in_input, [&](Key key, Input1D input)
		{
			result &= DoTryConsumeInput(in_input_receiver, input, in_input_device, in_flags);
			return false; // don't stop and process next
		});

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ?
			false:
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, Input1D in_input, InputDeviceInterface const* in_input_device, TaggedInputFlags in_flags) // check whether the axis is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle axis
		result &= DoTryConsumeInput(in_input_receiver, in_input, in_input_device, in_flags);
		// handle 'virtual related' keys/inputs
		InputTools::EnumerateRelatedInputs(in_input, [&](Key key, Input1D input)
		{
			result &= DoTryConsumeInput(in_input_receiver, key, in_input_device, in_flags);
			return false; // don't stop and process next
		},
		[&](Input2D input2D, Input1D input1D_x, Input1D input1D_y)
		{
			result &= DoTryConsumeInput(in_input_receiver, input2D, in_input_device, in_flags);
			return false; // don't stop and process next
		});

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ?
			false :
			result;
	}

	bool InputConsumptionCache::TryConsumeInput(InputReceiverInterface const* in_input_receiver, Input2D in_input, InputDeviceInterface const* in_input_device, TaggedInputFlags in_flags) // check whether the stick is still available and lock it for further requests (do the same for related inputs)
	{
		bool result = true;

		// handle stick
		result &= DoTryConsumeInput(in_input_receiver, in_input, in_input_device, in_flags);

		// handle 'virtual related' inputs
		InputTools::EnumerateRelatedInputs(in_input, [&](Input2D input2D, Input1D input1D_x, Input1D input1D_y)
		{
			result &= DoTryConsumeInput(in_input_receiver, input1D_x, in_input_device, in_flags);
			result &= DoTryConsumeInput(in_input_receiver, input1D_y, in_input_device, in_flags);
			return false; // don't stop and process next
		});

		return (all_inputs_consumer.has_value() && all_inputs_consumer.value() != in_input_receiver) ?
			false :
			result;
	}

	template<typename CONTAINER_TYPE, InputType INPUT_TYPE>
	bool TryInsertConsumedInput(CONTAINER_TYPE & inout_consumed_input, InputReceiverInterface const* in_input_receiver, INPUT_TYPE in_input, InputState_t<INPUT_TYPE> const* in_state, TaggedInputFlags in_flags)
	{
		auto consumed_input_key = std::make_pair(in_input, in_state);

		auto it = inout_consumed_input.find(consumed_input_key);

		// input is not required yet
		if (it == inout_consumed_input.end())
		{
			if (!HasAnyFlags(in_flags, TaggedInputFlags::CONSULT_ONLY))
				inout_consumed_input.emplace(consumed_input_key, in_input_receiver);
			return true;
		}
		// the same input receiver requires for the input. that's ok
		if (it->second == in_input_receiver)
			return true;
		// another receiver requires the input. that's a rejection
		return false;
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, Key in_input, InputDeviceInterface const* in_input_device, TaggedInputFlags in_flags)
	{
		auto const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return false;
		return TryInsertConsumedInput(consumed_keys, in_input_receiver, in_input, state, in_flags);
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, Input1D in_input, InputDeviceInterface const* in_input_device, TaggedInputFlags in_flags)
	{
		auto const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return false;
		return TryInsertConsumedInput(consumed_input1D, in_input_receiver, in_input, state, in_flags);
	}

	bool InputConsumptionCache::DoTryConsumeInput(InputReceiverInterface const* in_input_receiver, Input2D in_input, InputDeviceInterface const* in_input_device, TaggedInputFlags in_flags)
	{
		auto const* state = in_input_device->GetInputState(in_input);
		if (state == nullptr)
			return false;
		return TryInsertConsumedInput(consumed_input2D, in_input_receiver, in_input, state, in_flags);
	}

}; // namespace chaos
