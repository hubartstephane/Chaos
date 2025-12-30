namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<InputType INPUT_TYPE>
	class QueryInputRequest;

	using QueryKeyInputRequest = QueryInputRequest<Key>;
	using QueryInput1DRequest  = QueryInputRequest<Input1D>;
	using QueryInput2DRequest  = QueryInputRequest<Input2D>;

	enum class QueryInputRequestType;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* QueryInputRequestType: The requested input status
	*/

	enum class CHAOS_API QueryInputRequestType : int
	{
		None,
		Inactive,
		JustDeactivated,
		InactiveRepeated,
		Active,
		JustActivated,
		ActiveRepeated,
	};

	CHAOS_DECLARE_ENUM_METHOD(QueryInputRequestType, CHAOS_API);

	/**
	 * QueryInputRequest: a request that gets the value of an input
	 */

	template<InputType INPUT_TYPE>
	class QueryInputRequest : public InputRequestBase
	{
	public:

		using input_type = INPUT_TYPE;
		using state_type = InputState<input_type>;
		using value_type = InputValueType_t<input_type>;

		/** constructor */
		QueryInputRequest(input_type in_searched_input, state_type* in_out_state, value_type* in_out_value, QueryInputRequestType in_query_type):
			searched_input(in_searched_input),
			out_state(in_out_state),
			out_value(in_out_value),
			query_type(in_query_type)
		{
		}
		/** copy constructor */
		QueryInputRequest(QueryInputRequest const& src) = default;

		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override
		{
			// early exit
			if (searched_input == input_type::UNKNOWN)
				return InputRequestResult::Invalid;
			if (out_state == nullptr && out_value == nullptr && query_type == QueryInputRequestType::None) // this request is useless
				return InputRequestResult::Invalid;
			// find input
			state_type const* input_state = in_input_device->GetInputState(searched_input);
			if (input_state == nullptr)
				return InputRequestResult::Invalid; // abnormal (request for an input not handled by the receiver)
			// consum the key of the request (no one can use it anymore until next frame)
			if (!in_consumption_cache.TryConsumeInput(in_input_receiver, searched_input, in_input_device))
				return InputRequestResult::Rejected;

			value_type value = input_state->GetValue();

			// output results
			if (out_state != nullptr)
				*out_state = *input_state;
			if (out_value != nullptr)
				*out_value = value;

			// compute query result
			auto ConvertResultType = [](bool result)
			{
				return result? InputRequestResult::True: InputRequestResult::False;
			};

			switch (query_type)
			{
			case QueryInputRequestType::None:
				return InputRequestResult::True;
			case QueryInputRequestType::Inactive:
				return ConvertResultType(input_state->IsInactive());
			case QueryInputRequestType::JustDeactivated:
				return ConvertResultType(input_state->IsJustDeactivated());
			case QueryInputRequestType::InactiveRepeated:
				return ConvertResultType(input_state->IsInactiveRepeated());
			case QueryInputRequestType::Active:
				return ConvertResultType(input_state->IsActive());
			case QueryInputRequestType::JustActivated:
				return ConvertResultType(input_state->IsJustActivated());
			case QueryInputRequestType::ActiveRepeated:
				return ConvertResultType(input_state->IsActiveRepeated());
			default:
				assert(0);
			}
			return InputRequestResult::True; // whatever the value is, it's a success !
		}

		/** override */
		virtual bool IsRequestRelatedTo(Key in_input) const override
		{
			if constexpr (std::is_same_v<input_type, Key>)
				return (searched_input == in_input);
			return false;
		}
		/** override */
		virtual bool IsRequestRelatedTo(Input1D in_input) const override
		{
			if constexpr (std::is_same_v<input_type, Input1D>)
				return (searched_input == in_input);
			return false;
		}
		/** override */
		virtual bool IsRequestRelatedTo(Input2D in_input) const override
		{
			if constexpr (std::is_same_v<input_type, Input2D>)
				return (searched_input == in_input);
			return false;
		}
		/** override */
		virtual char const * GetDebugInfo(InputRequestDebugInfoStorage & debug_info_storage) const override
		{
			if (query_type == QueryInputRequestType::None)
			{
				if (out_state != nullptr || out_value != nullptr)
					std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Query[%s]", EnumToString(searched_input));
				else
					std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Misformed Query[%s]", EnumToString(searched_input));
			}
			else
			{
				std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "%s[%s]", EnumToString(query_type), EnumToString(searched_input));
			}
			return debug_info_storage.buffer;
		}

	protected:

		/** the concerned input */
		input_type searched_input;
		/** the state of the request */
		state_type* out_state = nullptr;
		/** the result of the request */
		value_type* out_value = nullptr;
		/** whether an inactive input is a success or not */
		QueryInputRequestType query_type = QueryInputRequestType::None;
	};

	/**
	 * Some standalone functions
	 */

	template<InputType INPUT_TYPE>
	QueryInputRequest<INPUT_TYPE> QueryInput(INPUT_TYPE in_input, QueryInputRequestType in_query_type = QueryInputRequestType::None)
	{
		return { in_input, nullptr, nullptr, in_query_type };
	}

	template<InputType INPUT_TYPE>
	QueryInputRequest<INPUT_TYPE> QueryInput(INPUT_TYPE in_input, InputValueType_t<INPUT_TYPE> *out_value, QueryInputRequestType in_query_type = QueryInputRequestType::None)
	{
		return { in_input, nullptr, out_value, in_query_type };
	}

	template<InputType INPUT_TYPE>
	QueryInputRequest<INPUT_TYPE> QueryInput(INPUT_TYPE in_input, InputState<INPUT_TYPE>* out_state, QueryInputRequestType in_query_type = QueryInputRequestType::None)
	{
		return { in_input, out_state, nullptr, in_query_type };
	}

	template<InputType T, typename ...PARAMS>
	auto Active(T in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::Active);
	}

	template<InputType T, typename ...PARAMS>
	auto JustActivated(T in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::JustActivated);
	}

	template<InputType T, typename ...PARAMS>
	auto ActiveRepeated(T in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::ActiveRepeated);
	}

	template<InputType T, typename ...PARAMS>
	auto Inactive(T in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::Inactive);
	}

	template<InputType T, typename ...PARAMS>
	auto JustDeactivated(T in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::JustDeactivated);
	}

	template<InputType T, typename ...PARAMS>
	auto InactiveRepeated(T in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::InactiveRepeated);
	}

#endif

}; // namespace chaos