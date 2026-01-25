namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputActionProcessor;

	using InputActionFunction = LightweightFunction<void()>;

	class InputAction;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* InputAction: defines an action to process (when some conditions are meet)
	*/

	class CHAOS_API InputAction
	{
	public:

		/** default constructor */
		InputAction(){}

		/** initializer constructor */
		template<std::invocable CALLABLE>
		InputAction(CALLABLE const & in_action_function, bool in_enabled = true):
			action_function(in_action_function),
			enabled(in_enabled)
		{}

		/** copy constructor */
		InputAction(InputAction const &) = default;

		/** processing function */
		void Process() const
		{
			if (enabled && action_function.IsValid())
				action_function();
		}

	public:

		/** the function to execute */
		InputActionFunction action_function;
		/** whether the action is to be really done */
		bool enabled = true;
	};

	/**
	* InputActionProcessor
	*/

	class CHAOS_API InputActionProcessor 
	{
	public:

		/** constructor */
		InputActionProcessor(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device);

		/** check for condition then process rquested action */
		virtual bool CheckAndProcess(InputConditionBase const& in_condition, char const* in_title, InputAction const & in_action = {});

		/** utility method to process Inputs directly without any explicit request */
		template<InputTypeExt INPUT_TYPE_EXT>
		bool CheckAndProcess(INPUT_TYPE_EXT in_input, char const* in_title, InputAction const& in_action)
		{
			return CheckAndProcess(JustActivated(in_input), in_title, in_action);
		}

	protected:

		/** Mark input as consumed in the WindowApplication's InputConsumptionCache */
		void MarkAllRequestInputsAsConsumedInApplicationCache(InputConditionBase const& in_condition);

	protected:
		
		/** the handled receiver */
		InputReceiverInterface const* input_receiver = nullptr;
		/** the input device considered */
		InputDeviceInterface const* input_device = nullptr;
	};

#endif

}; // namespace chaos