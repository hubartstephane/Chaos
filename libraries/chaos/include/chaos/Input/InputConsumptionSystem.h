namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputConsumptionSystem;

	enum class InputClaimResultStatus;
	CHAOS_DECLARE_ENUM_BOOL_METHOD(InputClaimResultStatus, CHAOS_API);

	enum class InputClaimResultFlags;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(InputClaimResultFlags, CHAOS_API);

	enum class InputClaimFlags;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(InputClaimFlags, CHAOS_API);

	template<AnyInputType ANY_INPUT_TYPE>
	class InputClaimResult;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InputClaimFlags: a bitfields to customize InputConsumptionSystem queries
	 */

	enum class InputClaimFlags : int
	{
		None = 0,
		ConsultOnly = 1 // consult cache (so call can fail) but do not lock resource for further calls
	};

	/**
	 * InputClaimResultStatus: indicate whether the query is sucessful or not
	 */

	enum class InputClaimResultStatus : int
	{
		Failure = 0,
		Success = 1
	};

	/**
	 * InputClaimResultFlags: bitfield used to describe what kind of event happened during a input query
	 */

	enum class InputClaimResultFlags : int
	{
		None             = 0,
		UnknownInput     = 1, // the request involved the input UNKNOWN
		KnownInput       = 2, // the request involved a KNOWN input
		UnhandledInput   = 4, // the request involved an input that was not handled by the InputDevice
		SuccessfulInput  = 8  // the request involved an input that was succesfully 
	};

	/**
	 * InputClaimResult: result coming from request to consumption cache
	 */

	template<AnyInputType ANY_INPUT_TYPE>
	class InputClaimResult
	{
	public:

		InputDeviceQueryResult<InputTypeToValueType_t<ANY_INPUT_TYPE>> input_result;
		/** indicates whether the response is a success or not */
		InputClaimResultStatus response_status = InputClaimResultStatus::Failure;
		/** the bitfield of what happened during request */
		InputClaimResultFlags response_flags = InputClaimResultFlags::None;
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
		InputClaimResult<ANY_INPUT_TYPE> ClaimInput(
			ANY_INPUT_TYPE const & in_input,
			InputReceiverInterface const* in_input_receiver,
			InputDeviceInterface const* in_input_device,
			InputClaimFlags in_claim_flags = InputClaimFlags::None
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
		InputClaimResultStatus TryConsumeInput(
			Key in_input, 
			InputReceiverInterface const* in_input_receiver, 
			InputClaimFlags in_claim_flags, 
			InputClaimResultFlags & out_response_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputClaimResultStatus TryConsumeInput(
			Input1D in_input, 
			InputReceiverInterface const* in_input_receiver, 
			InputClaimFlags in_claim_flags, 
			InputClaimResultFlags & out_response_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputClaimResultStatus TryConsumeInput(
			Input2D in_input, 
			InputReceiverInterface const* in_input_receiver, 
			InputClaimFlags in_claim_flags, 
			InputClaimResultFlags & out_response_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputClaimResultStatus TryConsumeInput(
			MappedInput1D const in_input,
			InputReceiverInterface const* in_input_receiver,
			InputClaimFlags in_claim_flags,
			InputClaimResultFlags& out_response_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		InputClaimResultStatus TryConsumeInput(
			MappedInput2D const in_input,
			InputReceiverInterface const* in_input_receiver,
			InputClaimFlags in_claim_flags,
			InputClaimResultFlags& out_response_flags);
		/** internal method that check whether an input or related inputs has already been consumed yet. Mark it as consumed */
		template<typename... PARAMS>
		requires HaveSameInputValueType<PARAMS...>
		InputClaimResultStatus TryConsumeInput(
			CombinedInput<PARAMS...> const& in_input,
			InputReceiverInterface const* in_input_receiver,
			InputClaimFlags in_claim_flags,
			InputClaimResultFlags& out_response_flags);

		/** internal method to consume a basic input */
		template<BasicInputType BASIC_INPUT_TYPE>
		InputClaimResultStatus DoTryConsumeInput(
			BASIC_INPUT_TYPE in_input,
			InputReceiverInterface const* in_input_receiver,
			InputClaimFlags in_claim_flags,
			InputClaimResultFlags& out_response_flags);

		/** consomme all inputs (with relations) for a given one */
		using ConsumeRelatedInputFunc = LightweightFunction<InputClaimResultStatus(InputReceiverInterface const* in_input_receiver, InputClaimFlags in_claim_flags, InputClaimResultFlags& out_response_flags)>;

		template<BasicInputType BASIC_INPUT_TYPE>
		InputClaimResultStatus TryConsumeInputAndRelated(
			BASIC_INPUT_TYPE in_input, 
			InputReceiverInterface const* in_input_receiver, 
			InputClaimFlags in_claim_flags,
			InputClaimResultFlags& out_response_flags,
			ConsumeRelatedInputFunc consume_related_input_func
		);

	protected:

		/** keys that are consumed */
		std::map<Key, InputReceiverInterface const*> consumed_keys;
		/** input1D that are consumed */
		std::map<Input1D, InputReceiverInterface const*> consumed_input1D;
		/** input2D that are consumed */
		std::map<Input2D, InputReceiverInterface const*> consumed_input2D;

		/** the receiver that claimed for all key */
		std::optional<InputReceiverInterface const*> all_key_consumer;
		/** the receiver that claimed for all input1D */
		std::optional<InputReceiverInterface const*> all_input1D_consumer;
		/** the receiver that claimed for all input2D */
		std::optional<InputReceiverInterface const*> all_input2D_consumer;
	};

#else

	template<AnyInputType ANY_INPUT_TYPE>
	InputClaimResult<ANY_INPUT_TYPE> InputConsumptionSystem::ClaimInput(ANY_INPUT_TYPE const & in_input, InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputClaimFlags in_claim_flags)
	{
		InputClaimResultFlags  response_flags  = InputClaimResultFlags::None;
		InputClaimResultStatus response_status = TryConsumeInput(in_input, in_input_receiver, in_claim_flags, response_flags);

		if (response_status == InputClaimResultStatus::Failure)
			return { {}, InputClaimResultStatus::Failure, InputClaimResultFlags::None }; // in case of FAILURE, ignore additionnal flags info. They are incomplete anyway (missing UNHANDLED_INPUT)

		auto input_result = in_input_device->GetInputState(in_input);

		// an unhandled event happens whenver neg_key or pos_key is defined but not handled by the event
		// in that case, the state returned is empty
		if (HasAnyFlags(response_flags, InputClaimResultFlags::KnownInput))
			if (!input_result.state.has_value())
				response_flags |= InputClaimResultFlags::UnhandledInput;

		return { input_result, InputClaimResultStatus::Success, response_flags };
	}

	template<typename... PARAMS>
	requires HaveSameInputValueType<PARAMS...>
	InputClaimResultStatus InputConsumptionSystem::TryConsumeInput(
		CombinedInput<PARAMS...> const & in_input,
		InputReceiverInterface const* in_input_receiver,
		InputClaimFlags in_claim_flags,
		InputClaimResultFlags& out_response_flags)
	{
		InputClaimResultFlags  response_flags = InputClaimResultFlags::None;
		InputClaimResultStatus response_status = InputClaimResultStatus::Success;

		std::apply([&](auto const & ... child_request)
		{
			auto TryConsumeInputChild = [&](auto const& child_request)
			{
				response_status &= TryConsumeInput(child_request, in_input_receiver, in_claim_flags, response_flags);
			};
			(TryConsumeInputChild(child_request), ...);

		}, in_input.child_inputs);

		return response_status;
	};

#endif

}; // namespace chaos