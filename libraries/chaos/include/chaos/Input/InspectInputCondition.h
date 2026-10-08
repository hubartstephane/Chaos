namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<AnyInputType ANY_INPUT_TYPE>
	class InspectInputCondition;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InspectInputCondition: a request that gets the value of an input
	 */

	template<AnyInputType ANY_INPUT_TYPE>
	class InspectInputCondition : public InputConditionBase
	{
	public:

		using input_type = ANY_INPUT_TYPE;
		using state_type = InputTypeToStateType_t<input_type>;
		using value_type = InputTypeToValueType_t<input_type>;

		/** constructor */
		InspectInputCondition(input_type in_input, state_type* in_out_state, value_type* in_out_value, InputStatusCheckType in_check_type):
			input(in_input),
			out_state(in_out_state),
			out_value(in_out_value),
			check_type(in_check_type)
		{
		}
		/** copy constructor */
		InspectInputCondition(InspectInputCondition const& src) = default;

		/** override */
		virtual InputConditionResult Check(InputConditionCheckParams const& in_params) const override
		{
			if (out_state == nullptr && out_value == nullptr && check_type == InputStatusCheckType::None) // this request is useless
				return InputConditionResult::Invalid;

			auto query_result = in_params.consumption_cache->QueryInputState(input, in_params.input_receiver, in_params.input_device);
			if (query_result.response_status == InputStateResponseStatus::Failure)
				return InputConditionResult::Rejected;
			if (!query_result.input_state.has_value())
				return InputConditionResult::Invalid; // abnormal (request for an input not handled by the receiver)

			return OuputDataAndReturnResult(&query_result.input_state.value());
		}

		/** output necessary data and get request result from the state and the query */
		InputConditionResult OuputDataAndReturnResult(state_type const * in_input_state) const
		{
			assert(in_input_state != nullptr);

			// output results
			if (out_state != nullptr)
				*out_state = *in_input_state;
			if (out_value != nullptr)
				*out_value = in_input_state->GetValue();

			// compute return value
			auto ConvertResultType = [](bool result)
			{
				return result ? InputConditionResult::True : InputConditionResult::False;
			};

			return in_input_state->CheckStatus(check_type)?
				InputConditionResult::True: 
				InputConditionResult::False;
		}

		/** override */
		virtual bool IsRequestRelatedTo(Key in_input) const override
		{
			if constexpr (std::is_same_v<input_type, Key>)
				return (input == in_input);
			if constexpr (std::is_same_v<input_type, MappedInput1D>)
				return (input.pos_key == in_input || input.neg_key == in_input);
			if constexpr (std::is_same_v<input_type, MappedInput2D>)
				return (input.left_key == in_input || input.right_key == in_input || input.up_key == in_input || input.down_key == in_input);
			return false;
		}
		/** override */
		virtual bool IsRequestRelatedTo(Input1D in_input) const override
		{
			if constexpr (std::is_same_v<input_type, Input1D>)
				return (input == in_input);
			return false;
		}
		/** override */
		virtual bool IsRequestRelatedTo(Input2D in_input) const override
		{
			if constexpr (std::is_same_v<input_type, Input2D>)
				return (input == in_input);
			return false;
		}
		/** override */
		virtual char const * GetDebugInfo(InputConditionDebugInfoStorage & debug_info_storage) const override
		{
			char input_buffer[256];

			if constexpr (std::is_same_v<input_type, MappedInput1D>)
			{
				std::snprintf(input_buffer, sizeof(input_buffer), "%s/%s",
					EnumToString(input.neg_key),
					EnumToString(input.pos_key));
			}
			else if constexpr (std::is_same_v<input_type, MappedInput2D>)
			{
				std::snprintf(input_buffer, sizeof(input_buffer), "%s/%s/%s/%s",
					EnumToString(input.left_key),
					EnumToString(input.right_key),
					EnumToString(input.up_key),
					EnumToString(input.down_key));
			}
			else if constexpr (BasicInputType<input_type>) // ignore mapped inputs
			{
				strcpy_s(input_buffer, sizeof(input_buffer), EnumToString(input));
			}

			if (check_type == InputStatusCheckType::None)
			{
				if (out_state != nullptr || out_value != nullptr)
					std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Query[%s]", input_buffer);
				else
					std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Misformed Query[%s]", input_buffer);
			}
			else
			{
				std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "%s[%s]", EnumToString(check_type), input_buffer);
			}
			return debug_info_storage.buffer;
		}

	protected:

		/** the concerned input */
		input_type input;
		/** the state of the request */
		state_type* out_state = nullptr;
		/** the result of the request */
		value_type* out_value = nullptr;
		/** whether an inactive input is a success or not */
		InputStatusCheckType check_type = InputStatusCheckType::None;
	};

	/**
	 * Some standalone functions
	 */
	
	template<AnyInputType ANY_INPUT_TYPE>
	InspectInputCondition<ANY_INPUT_TYPE> QueryInput(ANY_INPUT_TYPE in_input, InputStatusCheckType in_check_type = InputStatusCheckType::None)
	{
		return InspectInputCondition<ANY_INPUT_TYPE>(in_input, nullptr, nullptr, in_check_type);
	}

	template<AnyInputType ANY_INPUT_TYPE>
	InspectInputCondition<ANY_INPUT_TYPE> QueryInput(ANY_INPUT_TYPE in_input, InputTypeToValueType_t<ANY_INPUT_TYPE> *out_value, InputStatusCheckType in_check_type = InputStatusCheckType::None)
	{
		return InspectInputCondition<ANY_INPUT_TYPE>(in_input, nullptr, out_value, in_check_type);
	}

	template<AnyInputType ANY_INPUT_TYPE>
	InspectInputCondition<ANY_INPUT_TYPE> QueryInput(ANY_INPUT_TYPE in_input, InputTypeToStateType_t<ANY_INPUT_TYPE>* out_state, InputStatusCheckType in_check_type = InputStatusCheckType::None)
	{
		return InspectInputCondition<ANY_INPUT_TYPE>(in_input, out_state, nullptr, in_check_type);
	}
	
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto Active(ANY_INPUT_TYPE in_input, PARAMS&& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::Active);
	}
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto JustActivated(ANY_INPUT_TYPE in_input, PARAMS&& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::JustActivated);
	}
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto ActiveRepeated(ANY_INPUT_TYPE in_input, PARAMS&& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::ActiveRepeated);
	}
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto Inactive(ANY_INPUT_TYPE in_input, PARAMS&& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::Inactive);
	}
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto JustDeactivated(ANY_INPUT_TYPE in_input, PARAMS& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::JustDeactivated);
	}
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto InactiveRepeated(ANY_INPUT_TYPE in_input, PARAMS&& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::InactiveRepeated);
	}

#endif

}; // namespace chaos