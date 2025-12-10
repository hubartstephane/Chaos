namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

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
		{
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

		/** override */
		virtual bool IsRequestRelatedTo(Key in_input) const override
		{
			return IsRequestRelatedToImpl(in_input);
		}

		/** override */
		virtual bool IsRequestRelatedTo(Input1D in_input) const override
		{
			return IsRequestRelatedToImpl(in_input);
		}

		/** override */
		virtual bool IsRequestRelatedTo(Input2D in_input) const override
		{
			return IsRequestRelatedToImpl(in_input);
		}

	protected:

		template<typename INPUT_TYPE>
		bool IsRequestRelatedToImpl(INPUT_TYPE in_input) const
		{
			return std::apply([&](auto const & ... child_request) -> bool
			{
				return ((child_request.IsRequestRelatedTo(Input1D::UNKNOWN)) || ...);
			},
			child_input_requests);
		}

	protected:

		/** the input requests in the composition */
		std::tuple<PARAMS...> child_input_requests;
	};

	/**
	 * AndInputRequestResultAggregation: an utility class used to aggregate child results altogether for AND composition
	 */

	class CHAOS_API AndInputRequestResultAggregation
	{
	public:

		/** aggregation method */
		static InputRequestResult AggregateResult(std::optional<InputRequestResult> const& result, InputRequestResult child_result);
	};

	/**
	 * OrInputRequestResultAggregation: an utility class used to aggregate child results altogether for OR composition
	 */

	class CHAOS_API OrInputRequestResultAggregation
	{
	public:

		/** aggregation method */
		static InputRequestResult AggregateResult(std::optional<InputRequestResult> const& result, InputRequestResult child_result);
	};

	/**
	 * Utility methods
	 */

	template<InputRequestType... PARAMS>
	CompositeInputRequest<AndInputRequestResultAggregation, PARAMS...> And(PARAMS... params)
	{
		return CompositeInputRequest<AndInputRequestResultAggregation, PARAMS...>(std::forward<PARAMS>(params)...);
	}

	template<InputRequestType... PARAMS>
	CompositeInputRequest<OrInputRequestResultAggregation, PARAMS...> Or(PARAMS... params)
	{
		return CompositeInputRequest<OrInputRequestResultAggregation, PARAMS...>(std::forward<PARAMS>(params)...);
	}

#endif

}; // namespace chaos