namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputConsumptionSystem;

	enum class InputConsumeResultStatus;
	CHAOS_DECLARE_ENUM_BOOL_METHOD(InputConsumeResultStatus, CHAOS_API);

	enum class InputConsumeResultFlags;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(InputConsumeResultFlags, CHAOS_API);

	enum class InputConsumeFlags;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(InputConsumeFlags, CHAOS_API);

	template<AnyInputType ANY_INPUT_TYPE>
	class InputConsumeResult;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InputConsumeFlags: a bitfields to customize InputConsumptionSystem queries
	 */

	enum class InputConsumeFlags : int
	{
		None = 0,
		ConsultOnly = 1 // consult cache (so call can fail) but do not lock resource for further calls
	};

	/**
	 * InputConsumeResultStatus: indicate whether the query is sucessful or not
	 */

	enum class InputConsumeResultStatus : int
	{
		Failure = 0,
		Success = 1
	};

	/**
	 * InputConsumeResultFlags: bitfield used to describe what kind of event happened during a input query
	 */

	enum class InputConsumeResultFlags : int
	{
		None             = 0,
		UnknownInput     = 1, // the request involved the input UNKNOWN
		KnownInput       = 2, // the request involved a KNOWN input
		UnhandledInput   = 4, // the request involved an input that was not handled by the InputDevice
		SuccessfulInput  = 8  // the request involved an input that was succesfully 
	};

	/**
	 * InputConsumeResult: result coming from request to consumption cache
	 */

	template<AnyInputType ANY_INPUT_TYPE>
	class InputConsumeResult
	{
	public:

		InputDeviceQueryResult<InputTypeToValueType_t<ANY_INPUT_TYPE>> input_result;
		/** indicates whether the response is a success or not */
		InputConsumeResultStatus response_status = InputConsumeResultStatus::Failure;
		/** the bitfield of what happened during request */
		InputConsumeResultFlags response_flags = InputConsumeResultFlags::None;
	};

	/**
	 * InputConsumptionSystem: an object that keeps that trace of which inputs have been handled this frame
	 * 
	 * XXX: A trace of the input receiver consuming an input is kept. The same input may be further request but only by the same consumer
	 *      (nullptr is a valid consumer meaning no one will ever be allowed to consumed the input)
	 *      The same is true for SetAllInputConsumer(...)
	 */

	class CHAOS_API InputConsumptionSystem
	{

	public:

		/** check whether the input is still available and lock it for further requests (do the same for related inputs) */
		template<AnyInputType ANY_INPUT_TYPE>
		InputConsumeResult<ANY_INPUT_TYPE> ConsumeInput(
			ANY_INPUT_TYPE const & in_input,
			InputReceiverInterface const* in_input_receiver,
			InputDeviceInterface const* in_input_device,
			InputConsumeFlags in_consume_flags = InputConsumeFlags::None
		);

		/** all incomming input requests will be considered as consumed */
		bool SetAllInputConsumer(InputReceiverInterface const* in_input_receiver);
		/** all incomming key requests will be considered as consumed */
		bool SetAllKeyConsumer(InputReceiverInterface const* in_input_receiver);
		/** all incomming input1D requests will be considered as consumed */
		bool SetAllInput1DConsumer(InputReceiverInterface const* in_input_receiver);
		/** all incomming input2D requests will be considered as consumed */
		bool SetAllInput2DConsumer(InputReceiverInterface const* in_input_receiver);

		/** clear the cache */
		void Clear();

	protected:

		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputConsumeResultStatus TryConsumeInput(
			Key in_input, 
			InputReceiverInterface const* in_input_receiver, 
			InputConsumeFlags in_consume_flags, 
			InputConsumeResultFlags & out_response_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputConsumeResultStatus TryConsumeInput(
			Input1D in_input, 
			InputReceiverInterface const* in_input_receiver, 
			InputConsumeFlags in_consume_flags, 
			InputConsumeResultFlags & out_response_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputConsumeResultStatus TryConsumeInput(
			Input2D in_input, 
			InputReceiverInterface const* in_input_receiver, 
			InputConsumeFlags in_consume_flags, 
			InputConsumeResultFlags & out_response_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputConsumeResultStatus TryConsumeInput(
			MappedInput1D const in_input,
			InputReceiverInterface const* in_input_receiver,
			InputConsumeFlags in_consume_flags,
			InputConsumeResultFlags& out_response_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputConsumeResultStatus TryConsumeInput(
			MappedInput2D const in_input,
			InputReceiverInterface const* in_input_receiver,
			InputConsumeFlags in_consume_flags,
			InputConsumeResultFlags& out_response_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		template<typename... PARAMS>
		requires HaveSameInputValueType<PARAMS...>
		InputConsumeResultStatus TryConsumeInput(
			CombinedInput<PARAMS...> const& in_input,
			InputReceiverInterface const* in_input_receiver,
			InputConsumeFlags in_consume_flags,
			InputConsumeResultFlags& out_response_flags);

		/** internal method to consume a basic input */
		template<BasicInputType BASIC_INPUT_TYPE>
		InputConsumeResultStatus DoTryConsumeInput(
			BASIC_INPUT_TYPE in_input,
			InputReceiverInterface const* in_input_receiver,
			InputConsumeFlags in_consume_flags,
			InputConsumeResultFlags& out_response_flags);

		/** consomme all inputs (with relations) for a given one */
		using ConsumeRelatedInputFunc = LightweightFunction<InputConsumeResultStatus(InputReceiverInterface const* in_input_receiver, InputConsumeFlags in_consume_flags, InputConsumeResultFlags& out_response_flags)>;

		template<BasicInputType BASIC_INPUT_TYPE>
		InputConsumeResultStatus TryConsumeInputAndRelated(
			BASIC_INPUT_TYPE in_input, 
			InputReceiverInterface const* in_input_receiver, 
			InputConsumeFlags in_consume_flags,
			InputConsumeResultFlags& out_response_flags,
			ConsumeRelatedInputFunc consume_related_input_func
		);

	protected:

		/** keys that are consumed */
		std::map<Key, InputReceiverInterface const*> consumed_keys;
		/** input1D that are consumed */
		std::map<Input1D, InputReceiverInterface const*> consumed_input1D;
		/** input2D that are consumed */
		std::map<Input2D, InputReceiverInterface const*> consumed_input2D;

		/** the receiver that consumed all key */
		std::optional<InputReceiverInterface const*> all_key_consumer;
		/** the receiver that consumed all input1D */
		std::optional<InputReceiverInterface const*> all_input1D_consumer;
		/** the receiver that consumed all input2D */
		std::optional<InputReceiverInterface const*> all_input2D_consumer;
	};

#else

	template<AnyInputType ANY_INPUT_TYPE>
	InputConsumeResult<ANY_INPUT_TYPE> InputConsumptionSystem::ConsumeInput(ANY_INPUT_TYPE const & in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumeFlags in_consume_flags)
	{
		InputConsumeResultFlags  response_flags  = InputConsumeResultFlags::None;
		InputConsumeResultStatus response_status = TryConsumeInput(in_input, in_input_receiver, in_consume_flags, response_flags);

		if (response_status == InputConsumeResultStatus::Failure)
			return { {}, InputConsumeResultStatus::Failure, InputConsumeResultFlags::None }; // in case of FAILURE, ignore additionnal flags info. They are incomplete anyway (missing UNHANDLED_INPUT)

		auto input_result = in_input_device->GetInputState(in_input);

		// an unhandled event happens whenver neg_key or pos_key is defined but not handled by the event
		// in that case, the state returned is empty
		if (HasAnyFlags(response_flags, InputConsumeResultFlags::KnownInput))
			if (!input_result.state.has_value())
				response_flags |= InputConsumeResultFlags::UnhandledInput;

		return { input_result, InputConsumeResultStatus::Success, response_flags };
	}

	template<typename... PARAMS>
	requires HaveSameInputValueType<PARAMS...>
	InputConsumeResultStatus InputConsumptionSystem::TryConsumeInput(
		CombinedInput<PARAMS...> const & in_input,
		InputReceiverInterface const* in_input_receiver,
		InputConsumeFlags in_consume_flags,
		InputConsumeResultFlags& out_response_flags)
	{
		InputConsumeResultFlags  response_flags = InputConsumeResultFlags::None;
		InputConsumeResultStatus response_status = InputConsumeResultStatus::Success;

		std::apply([&](auto const & ... child_request)
		{
			auto TryConsumeInputChild = [&](auto const& child_request)
			{
				response_status &= TryConsumeInput(child_request, in_input_receiver, in_consume_flags, response_flags);
			};
			(TryConsumeInputChild(child_request), ...);

		}, in_input.child_inputs);

		return response_status;
	};

#endif

}; // namespace chaos