namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<InputType INPUT_TYPE>
	class OnEventInputActionProcessor;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * OnEventInputActionProcessor: a specialization of InputActionProcessor dedicated to window's events handling
	 */

	template<InputType INPUT_TYPE>
	class OnEventInputActionProcessor : public InputActionProcessor
	{
	public:

		/** constructor */
		OnEventInputActionProcessor(
			InputReceiverInterface const* in_input_receiver,
			InputDeviceInterface const* in_input_device,
			INPUT_TYPE const& in_input,
			InputConsumptionCache* in_consumption_cache
		):
			InputActionProcessor(in_input_receiver, in_input_device, in_consumption_cache),
			input(in_input)
		{
		}

		/** override */
		virtual bool CheckAndProcess(InputConditionBase const& in_condition, char const* in_title, InputAction const & in_action) override;

	protected:

		/** the input being processed */
		INPUT_TYPE input;
	};

#else

	template<InputType INPUT_TYPE>
	bool OnEventInputActionProcessor<INPUT_TYPE>::CheckAndProcess(InputConditionBase const& in_condition, char const* in_title, InputAction const& in_action)
	{
		// always consum input even if not related because, later on, maybe some composite request with the related key 
		// and some consumed input will be checked
		if (in_condition.Check({ input_receiver, input_device, consumption_cache }) == InputConditionResult::True)
		{
			if (in_condition.IsRequestRelatedTo(input)) // ignore all Actions with no relation with the handled event
			{
				// the internal 'consumption_cache' is not the same of the application
				// when an event occurs we iterate over all InputCondition/InputAction
				// we only handles thoses related to the incomming event (probably a key down)
				// we don't want to lock in the application consumption cache those ignored InputConditions
				// that's why we are working with a temporary Cache and only lock inputs for real in the application cache if the action is related to the event
				if (WindowApplication* window_application = Application::GetInstance())
					in_condition.Check({ nullptr, input_device, &window_application->GetInputConsumptionCache() });
				in_action.Process();
				return true; // stop
			}
		}
		return false;
	}

#endif

}; // namespace chaos