namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename INPUT_TYPE>
	class OnEventInputActionEnumerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * OnEventInputActionEnumerator: a specialization of InputActionEnumerator dedicated to window's events handling
	 */

	template<typename INPUT_TYPE>
	class OnEventInputActionEnumerator : public InputActionEnumerator
	{
	public:

		/** constructor */
		OnEventInputActionEnumerator(
			InputReceiverInterface const* in_input_receiver,
			InputDeviceInterface const* in_input_device,
			INPUT_TYPE const& in_input,
			InputConsumptionCache* in_consumption_cache
		):
			InputActionEnumerator(in_input_receiver, in_input_device),
			input(in_input),
			consumption_cache(in_consumption_cache)
		{
		}

		/** override */
		virtual bool CheckAndProcess(InputRequestBase const& in_request, char const* in_title, bool in_enabled, InputActionFunction in_func) override
		{
			// always consum input even if not related because, later on, maybe some composite request with the related key 
			// and some consumed input will be checked
			if (in_request.Check(input_receiver, input_device, *consumption_cache) == InputRequestResult::True)
			{
				if (in_request.IsRequestRelatedTo(input)) // ignore all Actions with no relation with the handled event
				{
					MarkAllRequestInputsAsConsumedInApplicationCache(in_request);

					if (in_enabled && in_func.IsValid())
						in_func();
					return true; // stop
				}
			}
			return false;
		}

	protected:

		/** the input being processed */
		INPUT_TYPE input;
		/** the consumption cache */
		InputConsumptionCache* consumption_cache = nullptr;		
	};

#endif

}; // namespace chaos