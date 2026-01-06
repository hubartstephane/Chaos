namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class CompositeInputType ;

	template<CompositeInputType COMPOSITE_INPUT_TYPE, InputRequestType... PARAMS>
	class CompositeInputRequest;

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
	 * CompositeInputRequest: a composite input request used to AND or OR request alltogether
	 */

	template<CompositeInputType COMPOSITE_INPUT_TYPE, InputRequestType... PARAMS>
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
					result = AggregateResult(result, child_result);
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

		/** utility function to build debug info string */
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
				bool parenthesis_required = (found_separator != 0 && found_separator != GetDebugInfoSeparator());

				if constexpr (INDEX > 0)
					if (!ConcatDebugInfo(GetDebugInfoSeparatorString()))
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
		InputRequestResult AggregateResult(std::optional<InputRequestResult> const& result, InputRequestResult child_result) const
		{
			auto ComputeAggregationResult = [&](std::initializer_list<InputRequestResult> const& ordered_values)
			{
				if (result.has_value())
					for (InputRequestResult r : ordered_values)
						if (child_result == r || result.value() == r)
							return r;
				return child_result;
			};

			if constexpr (COMPOSITE_INPUT_TYPE == CompositeInputType::And)
			{
				return ComputeAggregationResult({ InputRequestResult::Rejected, InputRequestResult::Invalid, InputRequestResult::False, InputRequestResult::True });
			}
			else if constexpr (COMPOSITE_INPUT_TYPE == CompositeInputType::Or)
			{
				return ComputeAggregationResult({ InputRequestResult::True, InputRequestResult::False, InputRequestResult::Invalid, InputRequestResult::Rejected });
			}
			else
			{
				static_assert(0);
			}
		}

		/** get separator */
		char GetDebugInfoSeparator() const
		{
			if constexpr (COMPOSITE_INPUT_TYPE == CompositeInputType::And)
			{
				return '&';
			}
			else if constexpr (COMPOSITE_INPUT_TYPE == CompositeInputType::Or)
			{
				return '|';
			}
			else
			{
				static_assert(0);
				return 0;
			}
		}

		/** get separator string */
		char const * GetDebugInfoSeparatorString() const
		{
			if constexpr (COMPOSITE_INPUT_TYPE == CompositeInputType::And)
			{
				return " & ";
			}
			else if constexpr (COMPOSITE_INPUT_TYPE == CompositeInputType::Or)
			{
				return " | ";
			}
			else
			{
				static_assert(0);
				return 0;
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
			return CompositeInputRequest<CompositeInputType::And, PARAMS...>(std::forward<PARAMS>(params)...);
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
			return CompositeInputRequest<CompositeInputType::Or, PARAMS...>(std::forward<PARAMS>(params)...);
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