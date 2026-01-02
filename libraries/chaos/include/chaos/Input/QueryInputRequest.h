namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<InputTypeExt INPUT_TYPE_EXT>
	class QueryInputRequest;

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

	template<InputTypeExt INPUT_TYPE_EXT>
	class QueryInputRequest : public InputRequestBase
	{
	public:

		using input_type = INPUT_TYPE_EXT;
		using state_type = InputState_t<input_type>;
		using value_type = InputValueType_t<input_type>;

		/** constructor */
		QueryInputRequest(input_type in_input, state_type* in_out_state, value_type* in_out_value, QueryInputRequestType in_query_type):
			input(in_input),
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
			if constexpr (InputType<input_type>) // only valid for Key, Input1D, Input2D
			{
				if (input == input_type::UNKNOWN)
					return InputRequestResult::Invalid;
			}
			if constexpr (std::is_same_v<input_type, MappedInput1D>)
			{
				if (input.pos_key == Key::UNKNOWN || input.neg_key == Key::UNKNOWN)
					return InputRequestResult::Invalid;			
			}
			if constexpr (std::is_same_v<input_type, MappedInput2D>)
			{
				if (input.left_key == Key::UNKNOWN || input.right_key == Key::UNKNOWN || input.up_key == Key::UNKNOWN || input.down_key == Key::UNKNOWN)
					return InputRequestResult::Invalid;
			}			
			if (out_state == nullptr && out_value == nullptr && query_type == QueryInputRequestType::None) // this request is useless
				return InputRequestResult::Invalid;

			// find and handle state
			if constexpr (std::is_same_v<input_type, MappedInput1D>)
			{
				KeyState neg_state;
				KeyState pos_state;

				auto internal_query = Or(
					QueryInput(input.neg_key, &neg_state),
					QueryInput(input.pos_key, &pos_state)
				);

				InputRequestResult internal_result = internal_query.Check(in_input_receiver, in_input_device, in_consumption_cache);
				if (internal_result == InputRequestResult::Invalid || internal_result == InputRequestResult::Rejected)
					return internal_result;

				Input1DState input_state;

				input_state.update_time = std::max(neg_state.update_time, pos_state.update_time); // this work for negative time (that correspond to unitialized state)

				if (neg_state.value)
					input_state.value -= 1.0f;
				if (pos_state.value)
					input_state.value += 1.0f;

				return OuputDataAndReturnResult(&input_state);
			}

			if constexpr (std::is_same_v<input_type, MappedInput2D>)
			{
				KeyState left_state;
				KeyState right_state;
				KeyState down_state;
				KeyState up_state;

				auto internal_query = Or(
					QueryInput(input.left_key, &left_state),
					QueryInput(input.right_key, &right_state),
					QueryInput(input.down_key, &down_state),
					QueryInput(input.up_key, &up_state)
				);

				InputRequestResult internal_result = internal_query.Check(in_input_receiver, in_input_device, in_consumption_cache);
				if (internal_result == InputRequestResult::Invalid || internal_result == InputRequestResult::Rejected)
					return internal_result;

				Input2DState input_state;

				// this work for negative time (that correspond to unitialized state)
				input_state.update_time = std::max(
					std::max(left_state.update_time, right_state.update_time),
					std::max(down_state.update_time, up_state.update_time)
				);

				if (left_state.value)
					input_state.value.x -= 1.0f;
				if (right_state.value)
					input_state.value.x += 1.0f;

				if (down_state.value)
					input_state.value.y -= 1.0f;
				if (up_state.value)
					input_state.value.y += 1.0f;

				return OuputDataAndReturnResult(&input_state);
			}

			if constexpr (InputType<input_type>) // ignore mapped inputs
			{
				state_type const* input_state = in_input_device->GetInputState(input);
				if (input_state == nullptr)
					return InputRequestResult::Invalid; // abnormal (request for an input not handled by the receiver)

				// consum the key of the request (no one can use it anymore until next frame)
				if (!in_consumption_cache.TryConsumeInput(in_input_receiver, input, in_input_device))
					return InputRequestResult::Rejected;

				return OuputDataAndReturnResult(input_state);
			}

			return OuputDataAndReturnResult(nullptr);
		}

		/** output necessary data and get request result from the state and the query */
		InputRequestResult OuputDataAndReturnResult(state_type const * in_input_state) const
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
				return result ? InputRequestResult::True : InputRequestResult::False;
			};

			switch (query_type)
			{
			case QueryInputRequestType::None:
				return InputRequestResult::True;
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
			return InputRequestResult::True; // whatever the value is, it's a success !
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
		virtual char const * GetDebugInfo(InputRequestDebugInfoStorage & debug_info_storage) const override
		{
			char input_buffer[256];

			if constexpr (std::is_same_v<input_type, MappedInput1D>)
			{
				std::snprintf(input_buffer, sizeof(input_buffer), "%s/%s",
					EnumToString(input.neg_key),
					EnumToString(input.pos_key));
			}
			if constexpr (std::is_same_v<input_type, MappedInput2D>)
			{
				std::snprintf(input_buffer, sizeof(input_buffer), "%s/%s/%s/%s",
					EnumToString(input.left_key),
					EnumToString(input.right_key),
					EnumToString(input.up_key),
					EnumToString(input.down_key));
			}
			if constexpr (InputType<input_type>) // ignore mapped inputs
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

	template<InputTypeExt INPUT_TYPE_EXT>
	QueryInputRequest<INPUT_TYPE_EXT> QueryInput(INPUT_TYPE_EXT in_input, QueryInputRequestType in_query_type = QueryInputRequestType::None)
	{
		return { in_input, nullptr, nullptr, in_query_type };
	}

	template<InputTypeExt INPUT_TYPE_EXT>
	QueryInputRequest<INPUT_TYPE_EXT> QueryInput(INPUT_TYPE_EXT in_input, InputValueType_t<INPUT_TYPE_EXT> *out_value, QueryInputRequestType in_query_type = QueryInputRequestType::None)
	{
		return { in_input, nullptr, out_value, in_query_type };
	}

	template<InputTypeExt INPUT_TYPE_EXT>
	QueryInputRequest<INPUT_TYPE_EXT> QueryInput(INPUT_TYPE_EXT in_input, InputState_t<INPUT_TYPE_EXT>* out_state, QueryInputRequestType in_query_type = QueryInputRequestType::None)
	{
		return { in_input, out_state, nullptr, in_query_type };
	}

	template<InputTypeExt INPUT_TYPE_EXT, typename ...PARAMS>
	auto Active(INPUT_TYPE_EXT in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::Active);
	}

	template<InputTypeExt INPUT_TYPE_EXT, typename ...PARAMS>
	auto JustActivated(INPUT_TYPE_EXT in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::JustActivated);
	}

	template<InputTypeExt INPUT_TYPE_EXT, typename ...PARAMS>
	auto ActiveRepeated(INPUT_TYPE_EXT in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::ActiveRepeated);
	}

	template<InputTypeExt INPUT_TYPE_EXT, typename ...PARAMS>
	auto Inactive(INPUT_TYPE_EXT in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::Inactive);
	}

	template<InputTypeExt INPUT_TYPE_EXT, typename ...PARAMS>
	auto JustDeactivated(INPUT_TYPE_EXT in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::JustDeactivated);
	}

	template<InputTypeExt INPUT_TYPE_EXT, typename ...PARAMS>
	auto InactiveRepeated(INPUT_TYPE_EXT in_input, PARAMS... params)
	{
		return QueryInput(in_input, std::forward<PARAMS>(params)..., QueryInputRequestType::InactiveRepeated);
	}

#endif

}; // namespace chaos