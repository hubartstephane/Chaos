namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename INPUT_SEARCH_KEY_TYPE, typename RESULT_TYPE>
	class InputValueRequest;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InputValueRequest: a request that gets the value of an input
	 */

	template<typename INPUT_SEARCH_KEY_TYPE, typename RESULT_TYPE>
	class InputValueRequest : public InputRequestBase
	{
	public:

		/** constructor */
		InputValueRequest(INPUT_SEARCH_KEY_TYPE const& in_searched_key, RESULT_TYPE& in_result):
			searched_key(in_searched_key),
			result(in_result)
		{
		}

		/** override */
		virtual InputRequestResult Check(InputEventReceiverInterface const* in_event_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override
		{
			if constexpr (std::is_same_v<INPUT_SEARCH_KEY_TYPE, Key>)
				if (!searched_key.IsValid())
					return InputRequestResult::Invalid;


			return InputRequestResult::Invalid;
		}

		/** override */
		virtual InputRequestResult Check(InputEventReceiverInterface const* in_event_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override
		{
			if constexpr (std::is_same_v<INPUT_SEARCH_KEY_TYPE, Key>)
				if (!searched_key.IsValid())
					return InputRequestResult::Invalid;

			auto const* input_state = in_input_device->GetInputState(searched_key);
			if (input_state == nullptr)
				return InputRequestResult::Rejected;

			result = input_state->GetValue();

			return InputRequestResult::True;
		}

	protected:

		/** the concerned input */
		INPUT_SEARCH_KEY_TYPE searched_key;
		/** the result of the request */
		RESULT_TYPE& result;
	};

	/**
	 * Some standalone functions
	 */

	CHAOS_API InputValueRequest<Key, bool> RequestInputValue(Key const& in_key, bool& in_result);

	CHAOS_API InputValueRequest<GamepadAxis, float> RequestInputValue(GamepadAxis const& in_axis, float& in_result);

	CHAOS_API InputValueRequest<GamepadStick, glm::vec2> RequestInputValue(GamepadStick const& in_stick, glm::vec2& in_result);

#endif

}; // namespace chaos