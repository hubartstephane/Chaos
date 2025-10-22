namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	concept InputRequestType = std::is_base_of_v<InputRequestBase, std::remove_cvref_t<T>>;

	template<typename RESULT_AGGREGATION_TYPE, InputRequestType... PARAMS>
	class CompositeInputRequest;

	class AndInputRequestResultAggregation;
	class OrInputRequestResultAggregation;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * CompositeInputRequest: a composite input request used to AND or OR request alltogether
	 */

	template<typename RESULT_AGGREGATION_TYPE, InputRequestType... PARAMS>
	class CompositeInputRequest : public InputRequestBase
	{
	public:
		
		/** constructor */
		CompositeInputRequest(PARAMS... params):
			child_input_requests(std::move(params)...)
		{}

		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override
		{
			std::optional<InputRequestResult> result;

			std::apply([&](auto const & ... child_request)
			{
				auto CheckChildInputRequest = [&](auto const& child_request)
				{
					InputRequestResult child_result = child_request.Check(in_input_receiver, in_key_event, in_input_device, in_consumption_cache);
					result = RESULT_AGGREGATION_TYPE::AggregateResult(result, child_result);
				};
				(CheckChildInputRequest(child_request), ...);

			}, child_input_requests);

			return result.value_or(InputRequestResult::False);
		}

		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override
		{
			std::optional<InputRequestResult> result;

			std::apply([&](auto const & ... child_request)
			{
				auto CheckChildInputRequest = [&](auto const& child_request)
				{
					InputRequestResult child_result = child_request.Check(in_input_receiver, in_input_device, in_consumption_cache);
					result = RESULT_AGGREGATION_TYPE::AggregateResult(result, child_result);
				};
				(CheckChildInputRequest(child_request), ...);

			}, child_input_requests);

			return result.value_or(InputRequestResult::False);
		}

	protected:

		/** the input requests in the composition */
		std::tuple<PARAMS...> child_input_requests;
	};

	/**
	 * AndInputRequestResultAggregation: an utility class used to aggregate child results altogether for AND composition
	 */

	class AndInputRequestResultAggregation
	{
	public:

		/** aggregation method */
		static InputRequestResult AggregateResult(std::optional<InputRequestResult> const & result, InputRequestResult child_result)
		{
			if (!result.has_value())
				return child_result;
			if (child_result == InputRequestResult::Rejected || result == InputRequestResult::Rejected)
				return InputRequestResult::Rejected;
			if (child_result == InputRequestResult::Invalid || result == InputRequestResult::Invalid)
				return InputRequestResult::Invalid;
			if (child_result == InputRequestResult::False || result == InputRequestResult::False)
				return InputRequestResult::False;
			return InputRequestResult::True;
		}
	};

	/**
	 * OrInputRequestResultAggregation: an utility class used to aggregate child results altogether for OR composition
	 */

	class OrInputRequestResultAggregation
	{
	public:

		/** aggregation method */
		static InputRequestResult AggregateResult(std::optional<InputRequestResult> const& result, InputRequestResult child_result)
		{
			if (!result.has_value())
				return child_result;
			if (child_result == InputRequestResult::True || result == InputRequestResult::True)
				return InputRequestResult::True;
			if (child_result == InputRequestResult::Rejected || result == InputRequestResult::Rejected)
				return InputRequestResult::Rejected;
			if (child_result == InputRequestResult::Invalid || result == InputRequestResult::Invalid)
				return InputRequestResult::Invalid;
			return InputRequestResult::False;
		}
	};

	/**
	 * Utility methods
	 */

	template<InputRequestType... PARAMS>
	CompositeInputRequest<AndInputRequestResultAggregation, PARAMS...> And(PARAMS... params)
	{
		return CompositeInputRequest<AndInputRequestResultAggregation, PARAMS...>(std::move(params)...);
	}

	template<InputRequestType... PARAMS>
	CompositeInputRequest<OrInputRequestResultAggregation, PARAMS...> Or(PARAMS... params)
	{
		return CompositeInputRequest<OrInputRequestResultAggregation, PARAMS...>(std::move(params)...);
	}

#endif

}; // namespace chaos