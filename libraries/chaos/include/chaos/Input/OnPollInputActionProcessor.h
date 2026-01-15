namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class OnPollInputActionProcessor;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * OnPollInputActionProcessor: a specialization of InputActionProcessor dedicated to input polling
	 */

	class CHAOS_API OnPollInputActionProcessor : public InputActionProcessor
	{
	public:

		/** constructor */
		OnPollInputActionProcessor(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache);

		/** override */
		virtual bool CheckAndProcess(InputConditionBase const& in_condition, char const* in_title, bool in_enabled, InputActionFunction in_func) override;

	protected:

		/** the consumption cache */
		InputConsumptionCache* consumption_cache = nullptr;
	};

#endif

}; // namespace chaos