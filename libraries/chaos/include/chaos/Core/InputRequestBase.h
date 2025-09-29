namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputRequestBase;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* InputRequestBase: a base request for any user input (keyboard, mouse, gamepad)
	*/

	class InputRequestBase
	{
	public:

		/** check whether the request matches a given key event */
		virtual bool Check(InputEventReceiverInterface const * in_event_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const;
		/** check whether the request matches a given input device state (polling) */
		virtual bool Check(InputEventReceiverInterface const* in_event_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const;
	};

#endif

}; // namespace chaos