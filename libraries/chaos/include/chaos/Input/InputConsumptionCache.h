namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputConsumptionCache;

	enum class InputStateQueryFlags;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(InputStateQueryFlags, CHAOS_API);

	enum class InputStateResponseFlags;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(InputStateResponseFlags, CHAOS_API);

	template<InputType INPUT_TYPE>
	class InputStateResponseType;

	CHAOS_GENERATE_CLASS_MAPPING_DECLARATION(InputStateResponse, InputTypeExt);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputStateResponse, Key, InputStateResponseType<Key>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputStateResponse, Input1D, InputStateResponseType<Input1D>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputStateResponse, Input2D, InputStateResponseType<Input2D>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputStateResponse, MappedInput1D, InputStateResponseType<Input1D>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputStateResponse, MappedInput2D, InputStateResponseType<Input2D>);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InputStateQueryFlags: a bitfields to customize InputConsumptionCache request
	 */

	enum class InputStateQueryFlags : int
	{
		NONE = 0,
		CONSULT_ONLY = 1 // consult cache (so call can fail) but do not lock resource for further calls
	};
	/**
	 * InputStateResponseFlags: bitfield used to describe what kind of event happened during a input query
	 */

	enum class InputStateResponseFlags : int
	{
		NONE              = 0,
		UNKNOWN_INPUT     = 1, // the request involved the input UNKNOWN
		KNOWN_INPUT       = 2, // the request involved a KNOWN input
		UNHANDLED_INPUT   = 4, // the request involved an input that was not handled by the InputDevice
		REJECTED_INPUT    = 8, // the request involved an input that was rejected (already in use)  
		SUCCESSFUL_INPUT  = 16 // the request involved an input that was succesfully 
	};

	/**
	 * InputStateResponseType: result coming from request to consumption cache
	 */

	template<InputType INPUT_TYPE>
	class InputStateResponseType
	{
	public:

		/** the state of the input */
		std::optional<InputState_t<INPUT_TYPE>> input_state;
		/** the bitfield of what happened during request */
		InputStateResponseFlags flags = InputStateResponseFlags::NONE;
	};

	/**
	 * InputConsumptionCache: an object that keeps that trace of which inputs have been handled this frame
	 * 
	 * XXX: A trace of the input receiver consuming an input is kept. The same input may be further request but only by the same consumer
	 *      (nullptr is a valid consumer meaning no one will ever be allowed to consumed the input)
	 *      The same is true for SetConsumeAllInputs(...)
	 */

	class CHAOS_API InputConsumptionCache
	{

	public:

		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		template<InputType INPUT_TYPE>
		InputStateResponse_t<INPUT_TYPE> QueryInputState(
			INPUT_TYPE in_input,
			InputReceiverInterface const* in_input_receiver,
			InputDeviceInterface const* in_input_device,
			InputStateQueryFlags in_flags = InputStateQueryFlags::NONE
		);

		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		InputStateResponse_t<MappedInput1D> QueryInputState(MappedInput1D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputStateQueryFlags in_flags = InputStateQueryFlags::NONE);
		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		InputStateResponse_t<MappedInput2D> QueryInputState(MappedInput2D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputStateQueryFlags in_flags = InputStateQueryFlags::NONE);

		/** all incomming input requests will be considered as consumed */
		void SetConsumeAllInputs(InputReceiverInterface const* in_input_receiver);

		/** clear the cache */
		void Clear();

	protected:

		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputStateResponseFlags TryConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputStateResponseFlags TryConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputStateResponseFlags TryConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags);

		/** internal method that check whether an input has already been consumed yet. Mark it as consumed */
		InputStateResponseFlags DoTryConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags);
		/** internal method that check whether an input has already been consumed yet. Mark it as consumed */
		InputStateResponseFlags DoTryConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags);
		/** internal method that check whether an input has already been consumed yet. Mark it as consumed */
		InputStateResponseFlags DoTryConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, InputStateQueryFlags in_flags);

		/** consomme all inputs (with relations) for a given one */
		template<InputType INPUT_TYPE>
		InputStateResponseFlags TryConsumeInputAndRelated(
			INPUT_TYPE in_input, 
			InputReceiverInterface const* in_input_receiver, 
			InputStateQueryFlags in_flags, 
			LightweightFunction<InputStateResponseFlags(InputStateResponseFlags)> consume_related_input_func
		);

		/** finalize query result */
		template<InputTypeExt INPUT_TYPE_EXT>
		InputStateResponse_t<INPUT_TYPE_EXT> InputStateResponseFinalization(
			INPUT_TYPE_EXT in_input,
			InputDeviceInterface const* in_input_device, 
			InputStateResponseFlags result_flags
		);

	protected:

		/** keys that are consumed */
		std::map<Key, InputReceiverInterface const*> consumed_keys;
		/** input1D that are consumed */
		std::map<Input1D, InputReceiverInterface const*> consumed_input1D;
		/** input2D that are consumed */
		std::map<Input2D, InputReceiverInterface const*> consumed_input2D;

		/** the receiver that claimed for all inputs */
		std::optional<InputReceiverInterface const*> all_inputs_consumer;
	};

#else

	template<InputType INPUT_TYPE>
	InputStateResponse_t<INPUT_TYPE> InputConsumptionCache::QueryInputState(INPUT_TYPE in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputStateQueryFlags in_flags)
	{
		InputStateResponseFlags result_flags = TryConsumeInput(in_input, in_input_receiver, in_flags);
		return InputStateResponseFinalization(in_input, in_input_device, result_flags);
	}

	template<InputTypeExt INPUT_TYPE_EXT>
	InputStateResponse_t<INPUT_TYPE_EXT> InputConsumptionCache::InputStateResponseFinalization(INPUT_TYPE_EXT in_input, InputDeviceInterface const* in_input_device, InputStateResponseFlags result_flags)
	{
		// in case of rejection we onlyu keep rejection bit
		if (HasAnyFlags(result_flags, InputStateResponseFlags::REJECTED_INPUT))
			return { {}, InputStateResponseFlags::REJECTED_INPUT };
		
		auto state = in_input_device->GetInputState(in_input);

		// an unhandled event happens whenver neg_key or pos_key is defined but not handled by the event
		// in that case, the state returned is empty
		if (HasAnyFlags(result_flags, InputStateResponseFlags::KNOWN_INPUT))
			if (!state.has_value())
				result_flags |= InputStateResponseFlags::UNHANDLED_INPUT;

		return { state, result_flags };
	}

#endif

}; // namespace chaos