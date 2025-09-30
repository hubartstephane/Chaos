namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputRequestBase;

	enum class InputRequestResult;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InputRequestResult: 
	 */

	enum class InputRequestResult : int
	{
		Invalid,   // the request is not properly initialized
		Rejected,  // the inputs for this request have already been consumed
		False,     // the request is not satisfied
		True       // the request is satisfied
	};

	/**
	* InputRequestBase: a base request for any user input (keyboard, mouse, gamepad)
	*/

	class InputRequestBase
	{
	public:

		/** check whether the request matches a given key event */
		virtual InputRequestResult Check(InputEventReceiverInterface const * in_event_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const;
		/** check whether the request matches a given input device state (polling) */
		virtual InputRequestResult Check(InputEventReceiverInterface const* in_event_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const;
	};

#endif

}; // namespace chaos