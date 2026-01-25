namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputActionProcessor;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

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