namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class OnEventInputActionEnumerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * OnEventInputActionEnumerator: a specialization of InputActionEnumerator dedicated to KeyEvent handling
	 */

	class CHAOS_API OnEventInputActionEnumerator : public InputActionEnumerator
	{
	public:

		/** constructor */
		OnEventInputActionEnumerator(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, KeyEventBase const& in_key_event, InputConsumptionCache* in_consumption_cache);

		/** override */
		virtual bool CheckAndProcess(InputRequestBase const& in_request, char const* in_title, bool in_enabled, InputActionFunction in_func) override;

	protected:

		/** the key event being processed */
		KeyEventBase const& key_event;
		/** the consumption cache */
		InputConsumptionCache* consumption_cache = nullptr;		
	};

#endif

}; // namespace chaos