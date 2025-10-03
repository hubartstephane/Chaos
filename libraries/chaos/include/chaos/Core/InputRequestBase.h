namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputRequestBase;
	class InputRequestDebugInfo;

	enum class InputRequestResult;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InputRequestResult: 
	 */

	enum class CHAOS_API InputRequestResult : int
	{
		Invalid,   // the request is not properly initialized
		Rejected,  // the inputs for this request have already been consumed
		False,     // the request is not satisfied
		True       // the request is satisfied
	};

	/**
	* InputRequestDebugInfo: some information to be displayed to user for debugging purpose
	*/

	class CHAOS_API InputRequestDebugInfo
	{
	public:

		/** the input concerned by the request */
		std::string input;
		/** the required modifiers string */
		std::string required_modifiers;
		/** the forbidden modifiers string */
		std::string forbidden_modifiers;
		/** the type of action */
		std::string action_type;
	};

	/**
	* InputRequestBase: a base request for any user input (keyboard, mouse, gamepad)
	*/

	class CHAOS_API InputRequestBase
	{
	public:

		/** some debugging information */
		virtual InputRequestDebugInfo GetDebugInfo() const;

		/** check whether the request matches a given key event */
		virtual InputRequestResult Check(InputReceiverInterface const * in_input_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const;
		/** check whether the request matches a given input device state (polling) */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const;
	};

#endif

}; // namespace chaos