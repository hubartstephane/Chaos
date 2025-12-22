namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename RESULT_AGGREGATION_TYPE, InputRequestType... PARAMS>
	class CompositeInputRequest;

	class AndInputRequestResultAggregation;
	class OrInputRequestResultAggregation;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * Some utility methods
	 */

	namespace details
	{
		/** iterate on string on search for | or & (that is not inside parenthesis) */
		char FindAccessibleOperator(char const* buffer);
	};

	/**
	 * CompositeInputRequest: a composite input request used to AND or OR request alltogether
	 */

	template<typename RESULT_AGGREGATION_TYPE, InputRequestType... PARAMS>
	class CompositeInputRequest : public InputRequestBase
	{
	public:
		
		/** constructor */
		CompositeInputRequest(PARAMS... params):
			child_input_requests(std::forward<PARAMS>(params)...)
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

		// By construction, we concat expressions
		//
		//     A --->   &   <--- B       or        (.....) --->   &   <--- (.....)
		// 
		//  If A contains an accessible & operator (an operator & that is not inside parenthesis)
		// 
		//     A = (....) & (....)
		//   
		//  We can simply concat A with no change (same is true if there is no accessible operator at all)
		// 
		//  But if A contains an accessible operator | we have to "protect" it with parenthesis 
		//
		//  If   A = (....) | (....)     ----->     use (A) = ((....) | (....)) for concatenation instead

		/** override */
		virtual char const* GetDebugInfo(InputRequestDebugInfoStorage & debug_info_storage) const override
		{
			ConcatChildDebugInfo<0>(debug_info_storage, 0);
			debug_info_storage.buffer[debug_info_storage.buffer_size - 1] = 0; // zero terminal, just in case
			return debug_info_storage.buffer;
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


		template<size_t INDEX>
		void ConcatChildDebugInfo(InputRequestDebugInfoStorage& debug_info_storage, size_t current_position) const
		{
			if constexpr (INDEX < std::tuple_size_v<decltype(child_input_requests)>)
			{	
				auto ConcatDebugInfo = [&](char const * src)
				{
					strncpy_s(
						debug_info_storage.buffer + current_position, 
						debug_info_storage.buffer_size - current_position,
						src,
						debug_info_storage.buffer_size - current_position - 1 // -1 for zero terminal string
					);
					current_position += strlen(src);
					return current_position < debug_info_storage.buffer_size - 1; // still some empty room in the buffer ?
				};

				InputRequestDebugInfoStorage child_debug_info_storage;

				InputRequestBase const& child_input_request = std::get<INDEX>(child_input_requests);
				child_input_request.GetDebugInfo(child_debug_info_storage);

				char found_separator = details::FindAccessibleOperator(child_debug_info_storage.buffer);
				bool parenthesis_required = (found_separator != 0 && found_separator != RESULT_AGGREGATION_TYPE::DebugInfoSeparator);

				if constexpr (INDEX > 0)
					if (!ConcatDebugInfo(RESULT_AGGREGATION_TYPE::DebugInfoSeparatorString))
						return;

				if (parenthesis_required && !ConcatDebugInfo("("))
					return;
				if (!ConcatDebugInfo(child_debug_info_storage.buffer))
					return;
				if (parenthesis_required && !ConcatDebugInfo(")"))
					return;

				ConcatChildDebugInfo<INDEX + 1>(debug_info_storage, current_position);
			}
		}

		template<typename INPUT_TYPE>
		bool IsRequestRelatedToImpl(INPUT_TYPE in_input) const
		{
			return std::apply([&](auto const & ... child_request) -> bool
			{
				return (child_request.IsRequestRelatedTo(in_input) || ...);
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

		/** a separator for aggregation */
		static char const DebugInfoSeparator = '&';
		static inline char const* DebugInfoSeparatorString = " & ";

		/** aggregation method */
		static InputRequestResult AggregateResult(std::optional<InputRequestResult> const& result, InputRequestResult child_result);
	};

	/**
	 * OrInputRequestResultAggregation: an utility class used to aggregate child results altogether for OR composition
	 */

	class CHAOS_API OrInputRequestResultAggregation
	{
	public:

		/** a separator for aggregation */
		static char const DebugInfoSeparator = '|';
		static inline char const * DebugInfoSeparatorString = " | ";

		/** aggregation method */
		static InputRequestResult AggregateResult(std::optional<InputRequestResult> const& result, InputRequestResult child_result);
	};

	/**
	 * Utility methods
	 */

	template<InputRequestType... PARAMS> requires (sizeof...(PARAMS) >= 2)
	CompositeInputRequest<AndInputRequestResultAggregation, PARAMS...> And(PARAMS... params)
	{
		return CompositeInputRequest<AndInputRequestResultAggregation, PARAMS...>(std::forward<PARAMS>(params)...);
	}

	template<InputRequestType... PARAMS> requires (sizeof...(PARAMS) >= 2)
	CompositeInputRequest<OrInputRequestResultAggregation, PARAMS...> Or(PARAMS... params)
	{
		return CompositeInputRequest<OrInputRequestResultAggregation, PARAMS...>(std::forward<PARAMS>(params)...);
	}

#endif

}; // namespace chaos