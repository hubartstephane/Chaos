namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename INPUT_SEARCH_KEY_TYPE, typename RESULT_TYPE>
	class ValueInputRequest;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ValueInputRequest: a request that gets the value of an input
	 */

	template<typename INPUT_SEARCH_KEY_TYPE, typename RESULT_TYPE>
	class ValueInputRequest : public InputRequestBase
	{
	public:

		/** constructor */
		ValueInputRequest(INPUT_SEARCH_KEY_TYPE in_searched_input, RESULT_TYPE& in_result):
			searched_input(in_searched_input),
			result(in_result)
		{
		}

		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override
		{
			// early exit
			if (searched_input == INPUT_SEARCH_KEY_TYPE::UNKNOWN)
				return InputRequestResult::Invalid;
			// find input
			auto const* input_state = in_input_device->GetInputState(searched_input);
			if (input_state == nullptr)
				return InputRequestResult::Invalid; // abnormal (request for an input not handled by the receiver)
			// consum the key of the request (no one can use it anymore until next frame)
			if (!in_consumption_cache.TryConsumeInput(in_input_receiver, searched_input, in_input_device))
				return InputRequestResult::Rejected;
			// get the result
			result = input_state->GetValue();

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
		virtual char const * GetDebugInfo(char* in_buffer, size_t in_size) const override
		{
			std::snprintf(in_buffer, in_size, "Query(%s)", EnumToString(searched_input));
			return in_buffer;
		}

	protected:

		/** the concerned input */
		INPUT_SEARCH_KEY_TYPE searched_input;
		/** the result of the request */
		RESULT_TYPE& result;
	};

	/**
	 * Some standalone functions
	 */

	CHAOS_API ValueInputRequest<Key, bool> QueryValue(Key in_key, bool& in_result);

	CHAOS_API ValueInputRequest<Input1D, float> QueryValue(Input1D in_axis, float& in_result);

	CHAOS_API ValueInputRequest<Input2D, glm::vec2> QueryValue(Input2D in_stick, glm::vec2& in_result);

#endif

}; // namespace chaos