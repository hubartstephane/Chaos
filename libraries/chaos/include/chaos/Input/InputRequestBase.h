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

		/** check whether the request matches a given input device state (polling) */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const;

		/** check whether the request is related to some input */
		virtual bool IsRequestRelatedTo(Key in_input) const;
		/** check whether the request is related to some input */
		virtual bool IsRequestRelatedTo(Input1D in_input) const;
		/** check whether the request is related to some input */
		virtual bool IsRequestRelatedTo(Input2D in_input) const;

		/** some debugging information */
		virtual InputRequestDebugInfo GetDebugInfo() const;
	};

#endif

}; // namespace chaos