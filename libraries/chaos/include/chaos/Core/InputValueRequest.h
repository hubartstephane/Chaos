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
		InputValueRequest(INPUT_SEARCH_KEY_TYPE const& in_searched_input, RESULT_TYPE& in_result):
			searched_input(in_searched_input),
			result(in_result)
		{
		}

		/** override */
		virtual InputRequestDebugInfo GetDebugInfo() const override
		{
			InputRequestDebugInfo result;

			if constexpr (std::is_same_v<INPUT_SEARCH_KEY_TYPE, Key>)
				result.input = searched_input.GetName();
			else if constexpr (std::is_same_v<INPUT_SEARCH_KEY_TYPE, GamepadAxis> || std::is_same_v<INPUT_SEARCH_KEY_TYPE, GamepadStick>)
				result.input = EnumToString(searched_input);
			result.action_type = "Query Value";

			return result;
		}

		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override
		{
			if constexpr (std::is_same_v<INPUT_SEARCH_KEY_TYPE, Key>) // this is only valid for key event
			{
				// early exit
				if (!searched_input.IsValid())
					return InputRequestResult::Invalid;
				// find input
				auto const* input_state = in_input_device->GetInputState(searched_input);
				if (input_state == nullptr)
					return InputRequestResult::Invalid; // abnormal (request for an input not handled by the receiver)
				// consum the key of the request (no one can use it anymore until next frame)
				if (!in_consumption_cache.TryConsumeInput(searched_input, in_input_device))
					return InputRequestResult::Rejected;
				// is this the input we are looking for
				if (in_key_event.key != searched_input)
					return InputRequestResult::False;
				// get the result
				result = in_key_event.IsKeyDownEvent();

				return InputRequestResult::True;
			}
			return InputRequestResult::Invalid;
		}

		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const override
		{
			// early exit
			if constexpr (std::is_same_v<INPUT_SEARCH_KEY_TYPE, Key>)
				if (!searched_input.IsValid())
					return InputRequestResult::Invalid;
			// find input
			auto const* input_state = in_input_device->GetInputState(searched_input);
			if (input_state == nullptr)
				return InputRequestResult::Invalid; // abnormal (request for an input not handled by the receiver)
			// consum the key of the request (no one can use it anymore until next frame)
			if (!in_consumption_cache.TryConsumeInput(searched_input, in_input_device))
				return InputRequestResult::Rejected;
			// get the result
			result = input_state->GetValue();

			return InputRequestResult::True;
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

	CHAOS_API InputValueRequest<Key, bool> RequestInputValue(Key const& in_key, bool& in_result);

	CHAOS_API InputValueRequest<GamepadAxis, float> RequestInputValue(GamepadAxis const& in_axis, float& in_result);

	CHAOS_API InputValueRequest<GamepadStick, glm::vec2> RequestInputValue(GamepadStick const& in_stick, glm::vec2& in_result);

#endif

}; // namespace chaos