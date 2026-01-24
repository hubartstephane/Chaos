namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class CompositeInputType ;

	template<CompositeInputType COMPOSITE_INPUT_TYPE, InputRequestType... PARAMS>
	class CompositeInputCondition;

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
	 * CompositeInputType: describes are input are to be combined together
	 */

	enum class CompositeInputType : int
	{
		Or,
		And
	};

	/**
	 * CompositeInputCondition: a composite input request used to AND or OR request alltogether
	 */

	template<CompositeInputType COMPOSITE_INPUT_TYPE, InputRequestType... PARAMS>
	class CompositeInputCondition : public InputConditionBase
	{
		CHAOS_GENERATE_VALUE_MAPPING_DECLARATION(InputDebugInfoSeparator, CompositeInputType);
		CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(InputDebugInfoSeparator, CompositeInputType::Or, char, '|');
		CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(InputDebugInfoSeparator, CompositeInputType::And, char, '&');
		CHAOS_GENERATE_VALUE_MAPPING_DECLARATION(InputDebugInfoSeparatorString, CompositeInputType);
		CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(InputDebugInfoSeparatorString, CompositeInputType::Or, char const*, " | ");
		CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(InputDebugInfoSeparatorString, CompositeInputType::And, char const*, " & ");

	public:
		
		/** constructor */
		CompositeInputCondition(PARAMS... params):
			child_input_requests(std::forward<PARAMS>(params)...)
		{
		}

		/** override */
		virtual InputConditionResult Check(InputConditionCheckParams const& in_params) const override
		{
			std::optional<InputConditionResult> result;

			std::apply([&](auto const & ... child_request)
			{
				auto CheckChildInputRequest = [&](auto const& child_request)
				{
					InputConditionResult child_result = child_request.Check(in_params);
					result = AggregateResult(result, child_result);
				};
				(CheckChildInputRequest(child_request), ...);

			}, child_input_requests);

			return result.value_or(InputConditionResult::False);
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
		virtual char const* GetDebugInfo(InputConditionDebugInfoStorage & debug_info_storage) const override
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

		/** utility function to build debug info string */
		template<size_t INDEX>
		void ConcatChildDebugInfo(InputConditionDebugInfoStorage& debug_info_storage, size_t current_position) const
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

				InputConditionDebugInfoStorage child_debug_info_storage;

				InputConditionBase const& child_input_request = std::get<INDEX>(child_input_requests);
				child_input_request.GetDebugInfo(child_debug_info_storage);

				char found_separator = details::FindAccessibleOperator(child_debug_info_storage.buffer);
				bool parenthesis_required = (found_separator != 0 && found_separator != InputDebugInfoSeparator_v<COMPOSITE_INPUT_TYPE>);

				if constexpr (INDEX > 0)
					if (!ConcatDebugInfo(InputDebugInfoSeparatorString_v<COMPOSITE_INPUT_TYPE>))
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


		/** helper function to find whether the InputRequest is related to input */
		template<InputType INPUT_TYPE>
		bool IsRequestRelatedToImpl(INPUT_TYPE in_input) const
		{
			return std::apply([&](auto const & ... child_request) -> bool
			{
				return (child_request.IsRequestRelatedTo(in_input) || ...);
			},
			child_input_requests);
		}

		/** combine results altogether */
		InputConditionResult AggregateResult(std::optional<InputConditionResult> const& result, InputConditionResult child_result) const
		{
			auto ComputeAggregationResult = [&](std::initializer_list<InputConditionResult> const& ordered_values)
			{
				if (result.has_value())
					for (InputConditionResult r : ordered_values)
						if (child_result == r || result.value() == r)
							return r;
				return child_result;
			};

			if constexpr (COMPOSITE_INPUT_TYPE == CompositeInputType::And)
			{
				return ComputeAggregationResult({ InputConditionResult::Rejected, InputConditionResult::Invalid, InputConditionResult::False, InputConditionResult::True });
			}
			else if constexpr (COMPOSITE_INPUT_TYPE == CompositeInputType::Or)
			{
				return ComputeAggregationResult({ InputConditionResult::True, InputConditionResult::False, InputConditionResult::Invalid, InputConditionResult::Rejected });
			}
			else
			{
				static_assert(0);
			}
		}

	protected:

		/** the input requests in the composition */
		std::tuple<PARAMS...> child_input_requests;
	};

	/**
	 * Utility methods
	 */

	template<typename... PARAMS> requires (sizeof...(PARAMS) >= 2)
	auto And(PARAMS... params)
	{
		if constexpr ((InputRequestType<PARAMS> && ...))
		{
			return CompositeInputCondition<CompositeInputType::And, PARAMS...>(std::forward<PARAMS>(params)...);
		}
		else if constexpr ((std::is_same_v<PARAMS, Key> && ...)) // every parameters have a bool value
		{
		}
		else if constexpr (((std::is_same_v<PARAMS, Input1D> || std::is_same_v<PARAMS, MappedInput1D>) && ...)) // every parameters have a float value
		{
		}
		else if constexpr (((std::is_same_v<PARAMS, Input2D> || std::is_same_v<PARAMS, MappedInput2D>) && ...)) // every parameters have a vec2 value
		{
		}
		else
		{
			static_assert(0); // incoherent parameters
		}		
	}

	template<typename... PARAMS> requires (sizeof...(PARAMS) >= 2)
	auto Or(PARAMS... params)
	{
		if constexpr ((InputRequestType<PARAMS> && ...))
		{
			return CompositeInputCondition<CompositeInputType::Or, PARAMS...>(std::forward<PARAMS>(params)...);
		}
		else if constexpr ((std::is_same_v<PARAMS, Key> && ...)) // every parameters have a bool value
		{
		}
		else if constexpr (((std::is_same_v<PARAMS, Input1D> || std::is_same_v<PARAMS, MappedInput1D>) && ...)) // every parameters have a float value
		{
		}
		else if constexpr (((std::is_same_v<PARAMS, Input2D> || std::is_same_v<PARAMS, MappedInput2D>) && ...)) // every parameters have a vec2 value
		{
		}
		else
		{
			static_assert(0); // incoherent parameters
		}
	}

#endif

}; // namespace chaos