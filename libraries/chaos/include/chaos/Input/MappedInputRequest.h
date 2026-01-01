namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GENERATE_IS_ANY_OF_CONCEPT(MappedInputType, MappedInput1D, MappedInput2D);

	template<MappedInputType INPUT_TYPE>
	class MappedInputRequest;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION



#if 0

	/**
	 * MappedInputRequest: a request to map 2 keys to a Input1D and 4 keys to an Input2D query
	 */

	template<MappedInputType MAPPED_INPUT_TYPE>
	class MappedInputRequest : public InputRequestBase
	{
	public:

		using mapped_input_type = MAPPED_INPUT_TYPE;
		using value_type = InputValueType_t<mapped_input_type>;

		/** copy constructor */
		MappedInputRequest(MappedInputRequest const& src) = default;
		/** constructor with initialization */
		MappedInputRequest(mapped_input_type const& in_input, value_type* in_out_value, QueryInputRequestType in_query_type):
			input(in_input),
			out_value(in_out_value),
			query_type(in_query_type)
		{}

		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override
		{
			if constexpr (std::is_same_v<mapped_input_type, MappedInput1D>)
			{
				InputState<Key> neg_state;
				InputState<Key> pos_state;

				bool neg_value = false;
				bool pos_value = false;

				auto internal_query = Or(
					QueryInput(input.neg_key, &neg_value),
					QueryInput(input.pos_key, &pos_value)
				);

				InputRequestResult internal_result = internal_query.Check(in_input_receiver, in_input_device, in_consumption_cache);
				if (internal_result == InputRequestResult::Invalid || internal_result == InputRequestResult::Rejected)
					return internal_result;

				if (out_value != nullptr)
				{
					if (neg_value && !pos_value)
						*out_value = -1.0f;
					else if (!neg_value && pos_value)
						*out_value = +1.0f;
				}
				return internal_result;
			}

			if constexpr (std::is_same_v<mapped_input_type, MappedInput2D>)
			{
				bool left_value  = false;
				bool right_value = false;
				bool up_value    = false;
				bool down_value  = false;

				auto internal_query = Or(
					QueryInput(input.left_key, &left_value),
					QueryInput(input.right_key, &right_value),
					QueryInput(input.up_key, &up_value),
					QueryInput(input.down_key, &down_value)
				);

				InputRequestResult internal_result = internal_query.Check(in_input_receiver, in_input_device, in_consumption_cache);
				if (internal_result == InputRequestResult::Invalid || internal_result == InputRequestResult::Rejected)
					return internal_result;

				if (out_value != nullptr)
				{
					if (left_value && !right_value)
						out_value->x = -1.0f;
					else if (!left_value && right_value)
						out_value->x = +1.0f;

					if (up_value && !down_value)
						out_value->y = +1.0f;
					else if (!up_value && down_value)
						out_value->y = -1.0f;
				}




				// use query_type here






				return internal_result;
			}
			return InputRequestResult::Invalid;
		}

		/** override */
		virtual bool IsRequestRelatedTo(Key in_input) const override
		{
			if constexpr (std::is_same_v<mapped_input_type, MappedInput1D>)
			{
				return
					(input.neg_key == in_input) ||
					(input.pos_key == in_input);
			}
			if constexpr (std::is_same_v<mapped_input_type, MappedInput2D>)
			{
				return
					(input.left_key  == in_input) ||
					(input.right_key == in_input) ||
					(input.up_key    == in_input) ||
					(input.down_key  == in_input);
			}
			return false;
		}

		/** override */
		virtual char const* GetDebugInfo(InputRequestDebugInfoStorage& debug_info_storage) const override
		{
			if constexpr (std::is_same_v<mapped_input_type, MappedInput1D>)
			{
				std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Mapping[%s/%s]", 
					EnumToString(input.neg_key),
					EnumToString(input.pos_key));
			}
			if constexpr (std::is_same_v<mapped_input_type, MappedInput2D>)
			{
				std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Mapping[%s/%s/%s/%s]", 
					EnumToString(input.left_key),
					EnumToString(input.right_key),
					EnumToString(input.up_key),
					EnumToString(input.down_key));
			}
			return debug_info_storage.buffer;
		}


	protected:

		/** the input */
		mapped_input_type input;
		/** the returned value */
		value_type* out_value = nullptr;
		/** whether an inactive input is a success or not */
		QueryInputRequestType query_type = QueryInputRequestType::None;
	};
#endif

	/**
	 * Standalone functions
	 */
#if 0
	template<MappedInputType MAPPED_INPUT_TYPE>
	MappedInputRequest<MAPPED_INPUT_TYPE> QueryInput(MAPPED_INPUT_TYPE const& in_input, QueryInputRequestType in_query_type = QueryInputRequestType::None)
	{
		return MappedInputRequest<MAPPED_INPUT_TYPE>(in_input, nullptr, in_query_type);
	}

	template<MappedInputType MAPPED_INPUT_TYPE>
	MappedInputRequest<MAPPED_INPUT_TYPE> QueryInput(MAPPED_INPUT_TYPE const& in_input, InputValueType_t<MAPPED_INPUT_TYPE>* out_value, QueryInputRequestType in_query_type = QueryInputRequestType::None)
	{
		return MappedInputRequest<MAPPED_INPUT_TYPE>(in_input, out_value, in_query_type);
	}
#endif

#endif

}; // namespace chaos