namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename INPUT_SEARCH_KEY_TYPE, typename STATE_TYPE, typename VALUE_TYPE>
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

	template<typename INPUT_SEARCH_KEY_TYPE, typename STATE_TYPE, typename VALUE_TYPE>
	class QueryInputRequest : public InputRequestBase
	{
	public:

		/** constructor */
		QueryInputRequest(INPUT_SEARCH_KEY_TYPE in_searched_input, STATE_TYPE* in_out_state, VALUE_TYPE * in_out_value, QueryInputRequestType in_query_type):
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
			if (searched_input == INPUT_SEARCH_KEY_TYPE::UNKNOWN)
				return InputRequestResult::Invalid;
			// find input
			STATE_TYPE const* input_state = in_input_device->GetInputState(searched_input);
			if (input_state == nullptr)
				return InputRequestResult::Invalid; // abnormal (request for an input not handled by the receiver)
			// consum the key of the request (no one can use it anymore until next frame)
			if (!in_consumption_cache.TryConsumeInput(in_input_receiver, searched_input, in_input_device))
				return InputRequestResult::Rejected;

			VALUE_TYPE value = input_state->GetValue();

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
			if constexpr (std::is_same_v<INPUT_SEARCH_KEY_TYPE, Key>)
				return (searched_input == in_input);
			return false;
		}
		/** override */
		virtual bool IsRequestRelatedTo(Input1D in_input) const override
		{
			if constexpr (std::is_same_v<INPUT_SEARCH_KEY_TYPE, Input1D>)
				return (searched_input == in_input);
			return false;
		}
		/** override */
		virtual bool IsRequestRelatedTo(Input2D in_input) const override
		{
			if constexpr (std::is_same_v<INPUT_SEARCH_KEY_TYPE, Input2D>)
				return (searched_input == in_input);
			return false;
		}
		/** override */
		virtual char const * GetDebugInfo(InputRequestDebugInfoStorage & debug_info_storage) const override
		{
			return "";
#if 0
			if (fail_on_inactive_input)
				std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Query+[%s]", EnumToString(searched_input));
			else
				std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Query[%s]", EnumToString(searched_input));
			return debug_info_storage.buffer;
#endif
		}

	protected:

		/** the concerned input */
		INPUT_SEARCH_KEY_TYPE searched_input;
		/** the state of the request */
		STATE_TYPE* out_state = nullptr;
		/** the result of the request */
		VALUE_TYPE* out_value = nullptr;
		/** whether an inactive input is a success or not */
		QueryInputRequestType query_type = QueryInputRequestType::None;
	};

	/**
	 * Some standalone functions
	 */

	CHAOS_API QueryInputRequest<Key, KeyState, bool> QueryInput(Key in_input, QueryInputRequestType in_query_type = QueryInputRequestType::None);

	CHAOS_API QueryInputRequest<Input1D, Input1DState, float> QueryInput(Input1D in_input, QueryInputRequestType in_query_type = QueryInputRequestType::None);

	CHAOS_API QueryInputRequest<Input2D, Input2DState, glm::vec2> QueryInput(Input2D in_input, QueryInputRequestType in_query_type = QueryInputRequestType::None);


	CHAOS_API QueryInputRequest<Key, KeyState, bool> QueryInput(Key in_input, bool* out_value, QueryInputRequestType in_query_type = QueryInputRequestType::None);

	CHAOS_API QueryInputRequest<Input1D, Input1DState, float> QueryInput(Input1D in_input, float* out_value, QueryInputRequestType in_query_type = QueryInputRequestType::None);

	CHAOS_API QueryInputRequest<Input2D, Input2DState, glm::vec2> QueryInput(Input2D in_input, glm::vec2* out_value, QueryInputRequestType in_query_type = QueryInputRequestType::None);


	CHAOS_API QueryInputRequest<Key, KeyState, bool> QueryInput(Key in_input, KeyState* out_state, QueryInputRequestType in_query_type = QueryInputRequestType::None);

	CHAOS_API QueryInputRequest<Input1D, Input1DState, float> QueryInput(Input1D in_input, Input1DState* out_state, QueryInputRequestType in_query_type = QueryInputRequestType::None);

	CHAOS_API QueryInputRequest<Input2D, Input2DState, glm::vec2> QueryInput(Input2D in_input, Input2DState* out_state, QueryInputRequestType in_query_type = QueryInputRequestType::None);

#endif

}; // namespace chaos