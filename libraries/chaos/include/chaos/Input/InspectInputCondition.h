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
		InspectInputCondition(input_type const & in_input, state_type* in_out_state, value_type* in_out_value, InputStatusCheckType in_check_type):
			input(in_input),
			out_state(in_out_state),
			out_value(in_out_value),
			check_type(in_check_type)
		{
		}
		/** copy constructor */
		InspectInputCondition(InspectInputCondition const& src) = default;

		/** override */
		virtual InputConditionResultType Check(InputConditionCheckParams const& in_params) const override
		{
			if (out_state == nullptr && out_value == nullptr && check_type == InputStatusCheckType::None) // this request is useless
				return InputConditionResultType::Invalid;

			auto request_result = in_params.consumption_system->ConsumeInput(input, in_params.input_receiver, in_params.input_device);
			if (request_result.response_status == InputConsumeResultStatus::Failure)
				return InputConditionResultType::Rejected;
			if (!request_result.input_result.state.has_value())
				return InputConditionResultType::Invalid; // abnormal (request for an input not handled by the receiver)

			return OuputDataAndReturnResult(request_result.input_result.state.value());
		}

		/** output necessary data and get request result from the state and the query */
		InputConditionResultType OuputDataAndReturnResult(state_type const & in_input_state) const
		{
			// output results
			if (out_state != nullptr)
				*out_state = in_input_state;
			if (out_value != nullptr)
				*out_value = in_input_state.value;

			// compute return value
			return in_input_state.CheckStatus(check_type)?
				InputConditionResultType::True: 
				InputConditionResultType::False;
		}

		/** override */
		virtual bool IsRequestRelatedTo(Key in_input) const override
		{
			return InputTools::IsBasicInputRelatedTo(in_input, input);
		}
		/** override */
		virtual bool IsRequestRelatedTo(Input1D in_input) const override
		{
			return InputTools::IsBasicInputRelatedTo(in_input, input);
		}
		/** override */
		virtual bool IsRequestRelatedTo(Input2D in_input) const override
		{
			return InputTools::IsBasicInputRelatedTo(in_input, input);
		}
		
		/** override */
		virtual char const * GetDebugInfo(InputConditionDebugInfoStorage & debug_info_storage) const override
		{
			char input_buffer[256];
			InputTools::GetInputString(input, input_buffer, sizeof(input_buffer));

			if (check_type == InputStatusCheckType::None)
			{
				if (out_state != nullptr || out_value != nullptr)
					std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Query(%s)", input_buffer);
				else
					std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Misformed Query(%s)", input_buffer);
			}
			else
			{
				std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "%s(%s)", EnumToString(check_type), input_buffer);
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
	InspectInputCondition<ANY_INPUT_TYPE> QueryInput(ANY_INPUT_TYPE const & in_input, InputStatusCheckType in_check_type = InputStatusCheckType::None)
	{
		return InspectInputCondition<ANY_INPUT_TYPE>(in_input, nullptr, nullptr, in_check_type);
	}

	template<AnyInputType ANY_INPUT_TYPE>
	InspectInputCondition<ANY_INPUT_TYPE> QueryInput(ANY_INPUT_TYPE const& in_input, InputTypeToValueType_t<ANY_INPUT_TYPE> *out_value, InputStatusCheckType in_check_type = InputStatusCheckType::None)
	{
		return InspectInputCondition<ANY_INPUT_TYPE>(in_input, nullptr, out_value, in_check_type);
	}

	template<AnyInputType ANY_INPUT_TYPE>
	InspectInputCondition<ANY_INPUT_TYPE> QueryInput(ANY_INPUT_TYPE const& in_input, InputTypeToStateType_t<ANY_INPUT_TYPE>* out_state, InputStatusCheckType in_check_type = InputStatusCheckType::None)
	{
		return InspectInputCondition<ANY_INPUT_TYPE>(in_input, out_state, nullptr, in_check_type);
	}
	
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto Active(ANY_INPUT_TYPE const& in_input, PARAMS&& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::Active);
	}
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto JustActivated(ANY_INPUT_TYPE const& in_input, PARAMS&& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::JustActivated);
	}
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto ActiveRepeated(ANY_INPUT_TYPE const& in_input, PARAMS&& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::ActiveRepeated);
	}
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto Inactive(ANY_INPUT_TYPE const& in_input, PARAMS&& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::Inactive);
	}
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto JustDeactivated(ANY_INPUT_TYPE const& in_input, PARAMS& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::JustDeactivated);
	}
	template<AnyInputType ANY_INPUT_TYPE, typename ...PARAMS>
	auto InactiveRepeated(ANY_INPUT_TYPE const& in_input, PARAMS&& ...params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., InputStatusCheckType::InactiveRepeated);
	}

#endif

}; // namespace chaos