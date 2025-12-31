namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Input1DMappingInfo;
	class Input2DMappingInfo;

	template<InputType INPUT_TYPE>
	class MappedInputRequest;

	using QueryMapped1DInputRequest = MappedInputRequest<Input1D>;
	using QueryMapped2DInputRequest = MappedInputRequest<Input2D>;

	CHAOS_DECLARE_CLASS_MAPPING(InputTypeToMappingInfo);
	CHAOS_SPECIALIZE_CLASS_MAPPING(InputTypeToMappingInfo, Input1D, Input1DMappingInfo);
	CHAOS_SPECIALIZE_CLASS_MAPPING(InputTypeToMappingInfo, Input2D, Input2DMappingInfo);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * Input1DMappingInfo: some key binding to get an input1D from keys
	 */

	class Input1DMappingInfo
	{
	public:

		/** the default mapping with keyboard */
		static Input1DMappingInfo const default_keyboard_mapping;
		/** the default mapping with dpad */
		static Input1DMappingInfo const default_dpad_mapping;

	public:

		/** the key bound to negative direction */
		Key neg_key = Key::UNKNOWN;
		/** the key bound to positive direction */
		Key pos_key = Key::UNKNOWN;
	};

	/**
	 * Input2DMappingInfo: some key binding to get an input2D from keys
	 */

	class Input2DMappingInfo
	{
	public:

		/** the default mapping with keyboard */
		static Input2DMappingInfo const default_keyboard_mapping;
		/** the default mapping with dpad */
		static Input2DMappingInfo const default_dpad_mapping;

	public:

		/** the key bound to left direction */
		Key left_key  = Key::UNKNOWN;
		/** the key bound to right direction */
		Key right_key = Key::UNKNOWN;
		/** the key bound to up direction */
		Key up_key    = Key::UNKNOWN;
		/** the key bound to down direction */
		Key down_key  = Key::UNKNOWN;
	};

	/**
	 * MappedInputRequest: a request to map 2 keys to a Input1D and 4 keys to an Input2D query
	 */

	template<InputType INPUT_TYPE>
	class MappedInputRequest : public InputRequestBase
	{
	public:

		using input_type = INPUT_TYPE;
		using value_type = InputValueType_t<input_type>;
		using mapping_info_type = InputTypeToMappingInfo_t<INPUT_TYPE>;

		/** copy constructor */
		MappedInputRequest(MappedInputRequest const& src) = default;
		/** constructor with initialization */
		MappedInputRequest(mapping_info_type const& in_mapping_info, value_type* in_out_value, QueryInputRequestType in_query_type):
			mapping_info(in_mapping_info),
			out_value(in_out_value),
			query_type(in_query_type)
		{}

		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override
		{
			if constexpr (std::is_same_v<INPUT_TYPE, Input1D>)
			{
				bool neg_value = false;
				bool pos_value = false;

				auto internal_query = Or(
					QueryInput(mapping_info.neg_key, &neg_value),
					QueryInput(mapping_info.pos_key, &pos_value)
				);

				InputRequestResult internal_result = internal_query.Check(in_input_receiver, in_input_device, in_consumption_cache);
				if (out_value != nullptr)
				{
					if (neg_value && !pos_value)
						*out_value = -1.0f;
					else if (!neg_value && pos_value)
						*out_value = +1.0f;
				}
				return internal_result;
			}

			if constexpr (std::is_same_v<INPUT_TYPE, Input2D>)
			{
				bool left_value  = false;
				bool right_value = false;
				bool up_value    = false;
				bool down_value  = false;

				auto internal_query = Or(
					QueryInput(mapping_info.left_key, &left_value),
					QueryInput(mapping_info.right_key, &right_value),
					QueryInput(mapping_info.up_key, &up_value),
					QueryInput(mapping_info.down_key, &down_value)
				);

				InputRequestResult internal_result = internal_query.Check(in_input_receiver, in_input_device, in_consumption_cache);
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
			if constexpr (std::is_same_v<INPUT_TYPE, Input1D>)
			{
				return
					(mapping_info.neg_key == in_input) ||
					(mapping_info.pos_key == in_input);
			}
			if constexpr (std::is_same_v<INPUT_TYPE, Input2D>)
			{
				return
					(mapping_info.left_key  == in_input) ||
					(mapping_info.right_key == in_input) ||
					(mapping_info.up_key    == in_input) ||
					(mapping_info.down_key  == in_input);
			}
			return false;
		}

		/** override */
		virtual char const* GetDebugInfo(InputRequestDebugInfoStorage& debug_info_storage) const override
		{
			if constexpr (std::is_same_v<INPUT_TYPE, Input1D>)
			{
				std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Mapping[%s/%s]", 
					EnumToString(mapping_info.neg_key), 
					EnumToString(mapping_info.pos_key));
			}
			if constexpr (std::is_same_v<INPUT_TYPE, Input2D>)
			{
				std::snprintf(debug_info_storage.buffer, debug_info_storage.buffer_size, "Mapping[%s/%s/%s/%s]", 
					EnumToString(mapping_info.left_key),
					EnumToString(mapping_info.right_key),
					EnumToString(mapping_info.up_key),
					EnumToString(mapping_info.down_key));
			}
			return debug_info_storage.buffer;
		}


	protected:

		/** the mapping information */
		mapping_info_type mapping_info;
		/** the returned value */
		value_type* out_value = nullptr;
		/** whether an inactive input is a success or not */
		QueryInputRequestType query_type = QueryInputRequestType::None;
	};

	CHAOS_API QueryMapped1DInputRequest QueryInput(Input1DMappingInfo const & in_input, QueryInputRequestType in_query_type = QueryInputRequestType::None);

	CHAOS_API QueryMapped2DInputRequest QueryInput(Input2DMappingInfo const& in_input, QueryInputRequestType in_query_type = QueryInputRequestType::None);

	CHAOS_API QueryMapped1DInputRequest QueryInput(Input1DMappingInfo const& in_input, float* out_value, QueryInputRequestType in_query_type = QueryInputRequestType::None);

	CHAOS_API QueryMapped2DInputRequest QueryInput(Input2DMappingInfo const& in_input, glm::vec2* out_value, QueryInputRequestType in_query_type = QueryInputRequestType::None);



#endif

}; // namespace chaos