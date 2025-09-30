namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class OnKeyEventInputActionEnumerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * OnKeyEventInputActionEnumerator: a specialization of InputActionEnumerator dedicated to KeyEvent handling
	 */

	class CHAOS_API OnKeyEventInputActionEnumerator : public InputActionEnumerator
	{
	public:

		/** constructor */
		OnKeyEventInputActionEnumerator(InputReceiverInterface const* in_input_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache);

		/** override */
		virtual bool CheckAndProcess(InputRequestBase const& in_request, char const* in_title, bool in_enabled, InputActionFunction in_func) override;

	protected:

		/** the key event being processed */
		KeyEventBase const& key_event;
		/** the input device considered */
		InputDeviceInterface const* input_device = nullptr;
		/** the consumption cache */
		InputConsumptionCache* consumption_cache = nullptr;		
	};

#endif

}; // namespace chaos