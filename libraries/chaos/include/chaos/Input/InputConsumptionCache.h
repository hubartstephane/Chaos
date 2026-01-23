namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputConsumptionCache;

	enum class QueryInputStateFlags;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(QueryInputStateFlags, CHAOS_API);

	enum class QueryInputStateResultFlags;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(QueryInputStateResultFlags, CHAOS_API);

	template<InputType INPUT_TYPE>
	class QueryInputStateResultBase;

	CHAOS_GENERATE_CLASS_MAPPING_DECLARATION(QueryInputStateResult, InputTypeExt);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(QueryInputStateResult, Key, QueryInputStateResultBase<Key>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(QueryInputStateResult, Input1D, QueryInputStateResultBase<Input1D>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(QueryInputStateResult, Input2D, QueryInputStateResultBase<Input2D>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(QueryInputStateResult, MappedInput1D, QueryInputStateResultBase<Input1D>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(QueryInputStateResult, MappedInput2D, QueryInputStateResultBase<Input2D>);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * QueryInputStateFlags: a bitfields to customize InputConsumptionCache request
	 */

	enum class QueryInputStateFlags : int
	{
		NONE = 0,
		CONSULT_ONLY = 1 // consult cache (so call can fail) but do not lock resource for further calls
	};
	/**
	 * QueryInputStateResultFlags: bitfield used to describe what kind of event happened during a input query
	 */

	enum class QueryInputStateResultFlags : int
	{
		NONE              = 0,
		UNKNOWN_INPUT     = 1, // the request involved the input UNKNOWN
		KNOWN_INPUT       = 2, // the request involved a KNOWN input
		UNHANDLED_INPUT   = 4, // the request involved an input that was not handled by the InputDevice
		REJECTED_INPUT    = 8, // the request involved an input that was rejected (already in use)  
		SUCCESSFULL_INPUT = 16 // the request involved an input that was succesfully 
	};

	/**
	 * QueryInputStateResultBase: result coming from request to consumption cache
	 */

	template<InputType INPUT_TYPE>
	class QueryInputStateResultBase
	{
	public:

		/** the state of the input */
		std::optional<InputState_t<INPUT_TYPE>> input_state;
		/** the bitfield of what happened during request */
		QueryInputStateResultFlags flags = QueryInputStateResultFlags::NONE;
	};

	/**
	 * Some functor  
	 **/

	using VirtualKeyEnumerationFunction = LightweightFunction<bool(Key, Input1D)>;

	using VirtualInputEnumerationFunction = LightweightFunction<bool(Input1D, Input1D, Input2D)>;

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
		QueryInputStateResult_t<INPUT_TYPE> QueryInputState(
			INPUT_TYPE in_input,
			InputReceiverInterface const* in_input_receiver,
			InputDeviceInterface const* in_input_device,
			QueryInputStateFlags in_flags = QueryInputStateFlags::NONE
		);

		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		QueryInputStateResult_t<MappedInput1D> QueryInputState(MappedInput1D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, QueryInputStateFlags in_flags = QueryInputStateFlags::NONE);
		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		QueryInputStateResult_t<MappedInput2D> QueryInputState(MappedInput2D in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, QueryInputStateFlags in_flags = QueryInputStateFlags::NONE);

		/** all incomming input requests will be considered as consumed */
		void SetConsumeAllInputs(InputReceiverInterface const* in_input_receiver);

		/** clear the cache */
		void Clear();

	protected:

		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		QueryInputStateResultFlags CheckAndConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		QueryInputStateResultFlags CheckAndConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		QueryInputStateResultFlags CheckAndConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags);

		/** internal method that check whether an input has already been consumed yet. Mark it as consumed */
		QueryInputStateResultFlags DoCheckAndConsumeInput(Key in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags);
		/** internal method that check whether an input has already been consumed yet. Mark it as consumed */
		QueryInputStateResultFlags DoCheckAndConsumeInput(Input1D in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags);
		/** internal method that check whether an input has already been consumed yet. Mark it as consumed */
		QueryInputStateResultFlags DoCheckAndConsumeInput(Input2D in_input, InputReceiverInterface const* in_input_receiver, QueryInputStateFlags in_flags);

		/** consomme all inputs (with relations) for a given one */
		template<InputType INPUT_TYPE>
		QueryInputStateResultFlags DoCheckAndConsumeInputAndRelated(
			INPUT_TYPE in_input, 
			InputReceiverInterface const* in_input_receiver, 
			QueryInputStateFlags in_flags, 
			LightweightFunction<QueryInputStateResultFlags(QueryInputStateResultFlags)> check_related_input_func
		);

		/** finalize query result */
		template<InputTypeExt INPUT_TYPE_EXT>
		QueryInputStateResult_t<INPUT_TYPE_EXT> QueryInputStateResultFinalization(
			INPUT_TYPE_EXT in_input,
			InputDeviceInterface const* in_input_device, 
			QueryInputStateResultFlags result_flags
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
	QueryInputStateResult_t<INPUT_TYPE> InputConsumptionCache::QueryInputState(INPUT_TYPE in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, QueryInputStateFlags in_flags)
	{
		QueryInputStateResultFlags result_flags = CheckAndConsumeInput(in_input, in_input_receiver, in_flags);
		return QueryInputStateResultFinalization(in_input, in_input_device, result_flags);
	}

	template<InputTypeExt INPUT_TYPE_EXT>
	QueryInputStateResult_t<INPUT_TYPE_EXT> InputConsumptionCache::QueryInputStateResultFinalization(INPUT_TYPE_EXT in_input, InputDeviceInterface const* in_input_device, QueryInputStateResultFlags result_flags)
	{
		// in case of rejection we onlyu keep rejection bit
		if (HasAnyFlags(result_flags, QueryInputStateResultFlags::REJECTED_INPUT))
			return { {}, QueryInputStateResultFlags::REJECTED_INPUT };
		
		auto state = in_input_device->GetInputState(in_input);

		// an unhandled event happens whenver neg_key or pos_key is defined but not handled by the event
		// in that case, the state returned is empty
		if (HasAnyFlags(result_flags, QueryInputStateResultFlags::KNOWN_INPUT))
			if (!state.has_value())
				result_flags |= QueryInputStateResultFlags::UNHANDLED_INPUT;

		return { state, result_flags };
	}

#endif

}; // namespace chaos