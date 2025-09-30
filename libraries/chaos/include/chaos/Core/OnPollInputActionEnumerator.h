namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class OnPollInputActionEnumerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * OnPollInputActionEnumerator: a specialization of InputActionEnumerator dedicated to input polling
	 */

	class CHAOS_API OnPollInputActionEnumerator : public InputActionEnumerator
	{
	public:

		/** constructor */
		OnPollInputActionEnumerator(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache);

		/** override */
		virtual bool CheckAndProcess(InputRequestBase const& in_request, char const* in_title, bool in_enabled, InputActionFunction in_func) override;

	protected:

		/** the input device considered */
		InputDeviceInterface const* input_device = nullptr;
		/** the consumption cache */
		InputConsumptionCache* consumption_cache = nullptr;
	};

#endif

}; // namespace chaos