namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputConditionBase;

	class InputConditionDebugInfoStorage;

	enum class InputConditionResult;

	CHAOS_GENERATE_INHERIT_FROM_ANY_CONCEPT(InputRequestType, InputConditionBase);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InputConditionResult: 
	 */

	enum class CHAOS_API InputConditionResult : int
	{
		Invalid,   // the request is not properly initialized
		Rejected,  // the inputs for this request have already been consumed
		False,     // the request is not satisfied
		True       // the request is satisfied
	};

	/**
	 * InputConditionDebugInfoStorage: a structure to get debug information
	 */

	class CHAOS_API InputConditionDebugInfoStorage
	{
	public:

		/** the size of the buffer */
		static constexpr size_t buffer_size = 256;

		/** the buffer to store the string */
		char buffer[buffer_size];
	};

	/**
	* InputConditionCheckContext: a class that contains all information required for InputConditionBase::Check(...) methods
	*/

	class InputConditionCheckParams
	{
	public:

		/** the input receiver that requested the condition */
		InputReceiverInterface const * input_receiver = nullptr;
		/** the input device that provides all inputs */
		InputDeviceInterface const * input_device = nullptr;
		/** the consumption cache to use */
		InputConsumptionCache * consumption_cache = nullptr;
	};

	/**
	* InputConditionBase: a base request for any user input (keyboard, mouse, gamepad)
	*/

	class CHAOS_API InputConditionBase
	{
	public:

		/** check whether the request matches a given input device state (polling) */
		virtual InputConditionResult Check(InputConditionCheckParams const & in_params) const;

		/** check whether the request is related to some input */
		virtual bool IsRequestRelatedTo(Key in_input) const;
		/** check whether the request is related to some input */
		virtual bool IsRequestRelatedTo(Input1D in_input) const;
		/** check whether the request is related to some input */
		virtual bool IsRequestRelatedTo(Input2D in_input) const;

		/** some debugging information */
		virtual char const * GetDebugInfo(InputConditionDebugInfoStorage & debug_info_storage) const;
	};

#endif

}; // namespace chaos