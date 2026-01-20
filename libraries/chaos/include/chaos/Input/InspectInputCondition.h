namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<InputTypeExt INPUT_TYPE_EXT>
	class InspectInputCondition;

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
	 * InspectInputCondition: a request that gets the value of an input
	 */

	template<InputTypeExt INPUT_TYPE_EXT>
	class InspectInputCondition : public InputConditionBase
	{
	public:

		using input_type = INPUT_TYPE_EXT;
		using state_type = InputState_t<input_type>;
		using value_type = InputValueType_t<input_type>;

		/** constructor */
		InspectInputCondition(input_type in_input, state_type* in_out_state, value_type* in_out_value, QueryInputRequestType in_query_type):
			input(in_input),
			out_state(in_out_state),
			out_value(in_out_value),
			query_type(in_query_type)
		{
		}
		/** copy constructor */
		InspectInputCondition(InspectInputCondition const& src) = default;

		/** override */
		virtual InputConditionResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override
		{
			if (out_state == nullptr && out_value == nullptr && query_type == QueryInputRequestType::None) // this request is useless
				return InputConditionResult::Invalid;

			// find and handle state
			std::optional<state_type> input_state = in_input_device->GetInputState(input);
			if (!input_state.has_value())
				return InputConditionResult::Invalid; // abnormal (request for an input not handled by the receiver)

			// consum the key of the request (no one can use it anymore until next frame)
			if (!in_consumption_cache.TryConsumeInput(in_input_receiver, in_input_device, input))
				return InputConditionResult::Rejected;

			return OuputDataAndReturnResult(&input_state.value());
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

			switch (query_type)
			{
			case QueryInputRequestType::None:
				return InputConditionResult::True;
			case QueryInputRequestType::Inactive:
				return ConvertResultType(in_input_state->IsInactive());
			case QueryInputRequestType::JustDeactivated:
				return ConvertResultType(in_input_state->IsJustDeactivated());
			case QueryInputRequestType::InactiveRepeated:
				return ConvertResultType(in_input_state->IsInactiveRepeated());
			case QueryInputRequestType::Active:
				return ConvertResultType(in_input_state->IsActive());
			case QueryInputRequestType::JustActivated:
				return ConvertResultType(in_input_state->IsJustActivated());
			case QueryInputRequestType::ActiveRepeated:
				return ConvertResultType(in_input_state->IsActiveRepeated());
			default:
				assert(0);
			}
			return InputConditionResult::True; // whatever the value is, it's a success !
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
			else if constexpr (InputType<input_type>) // ignore mapped inputs
			{
				strcpy_s(input_buffer, sizeof(input_buffer), EnumToString(input));
			}

			if (query_type == QueryInputRequestType::None)
			{
				if (out_state != nullptr || out_value != nullptr)
					std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Query[%s]", input_buffer);
				else
					std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Misformed Query[%s]", input_buffer);
			}
			else
			{
				std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "%s[%s]", EnumToString(query_type), input_buffer);
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
		QueryInputRequestType query_type = QueryInputRequestType::None;
	};

	/**
	 * Some standalone functions
	 */
	
#define CHAOS_DECLARE_QUERY_INPUT(INPUT_TYPE, QUERY_INPUT_REQUEST_PARAM_TYPE)\
	InspectInputCondition<QUERY_INPUT_REQUEST_PARAM_TYPE> QueryInput(INPUT_TYPE in_input, QueryInputRequestType in_query_type = QueryInputRequestType::None);\
	InspectInputCondition<QUERY_INPUT_REQUEST_PARAM_TYPE> QueryInput(INPUT_TYPE in_input, InputValueType_t<INPUT_TYPE> *out_value, QueryInputRequestType in_query_type = QueryInputRequestType::None);\
	InspectInputCondition<QUERY_INPUT_REQUEST_PARAM_TYPE> QueryInput(INPUT_TYPE in_input, InputState_t<INPUT_TYPE>* out_state, QueryInputRequestType in_query_type = QueryInputRequestType::None);\
	template<typename ...PARAMS>\
	auto Active(INPUT_TYPE in_input, PARAMS... params)\
	{\
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::Active);\
	}\
	template<typename ...PARAMS>\
	auto JustActivated(INPUT_TYPE in_input, PARAMS... params)\
	{\
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::JustActivated);\
	}\
	template<typename ...PARAMS>\
	auto ActiveRepeated(INPUT_TYPE in_input, PARAMS... params)\
	{\
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::ActiveRepeated);\
	}\
	template<typename ...PARAMS>\
	auto Inactive(INPUT_TYPE in_input, PARAMS... params)\
	{\
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::Inactive);\
	}\
	template<typename ...PARAMS>\
	auto JustDeactivated(INPUT_TYPE in_input, PARAMS... params)\
	{\
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::JustDeactivated);\
	}\
	template<typename ...PARAMS>\
	auto InactiveRepeated(INPUT_TYPE in_input, PARAMS... params)\
	{\
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::InactiveRepeated);\
	}

	CHAOS_DECLARE_QUERY_INPUT(Key, Key);
	CHAOS_DECLARE_QUERY_INPUT(Input1D, Input1D);
	CHAOS_DECLARE_QUERY_INPUT(Input2D, Input2D);
	CHAOS_DECLARE_QUERY_INPUT(MappedInput1D, MappedInput1D);
	CHAOS_DECLARE_QUERY_INPUT(MappedInput2D, MappedInput2D);

#undef CHAOS_DECLARE_QUERY_INPUT

#endif

}; // namespace chaos