namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputRequestBase;

	enum class InputRequestResult;

	template<typename T>
	concept InputRequestType = std::is_base_of_v<InputRequestBase, std::remove_cvref_t<T>>;

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
		virtual char const * GetDebugInfo(char* in_buffer, size_t in_size) const;
	};

#endif

}; // namespace chaos