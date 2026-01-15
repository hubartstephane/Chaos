namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputActionEnumerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Some aliases
	*/

	using InputActionFunction = LightweightFunction<void()>;

	/**
	* InputActionEnumerator
	*/

	class CHAOS_API InputActionEnumerator 
	{
	public:

		/** constructor */
		InputActionEnumerator(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device);


#if 1
		template<InputType INPUT_TYPE>
		bool CheckAndProcess(INPUT_TYPE in_input, char const* in_title, bool in_enabled, InputActionFunction in_action_func);

		template<InputType INPUT_TYPE>
		bool CheckAndProcess(INPUT_TYPE in_input, char const* in_title, InputActionFunction in_action_func);
#endif

		/** the operator */
		virtual bool CheckAndProcess(InputRequestBase const& in_request, char const* in_title, bool in_enabled, InputActionFunction in_action_func);

		/** lighter function, with in_enabled defaulted to true */
		bool CheckAndProcess(InputRequestBase const& in_request, char const* in_title, InputActionFunction in_action_func = {});

	protected:

		/** Mark input as consumed in the WindowApplication's InputConsumptionCache */
		void MarkAllRequestInputsAsConsumedInApplicationCache(InputRequestBase const& in_request);

	protected:
		
		/** the handled receiver */
		InputReceiverInterface const* input_receiver = nullptr;
		/** the input device considered */
		InputDeviceInterface const* input_device = nullptr;
	};

#else

#if 1
	template<InputType INPUT_TYPE>
	bool InputActionEnumerator::CheckAndProcess(INPUT_TYPE in_input, char const* in_title, InputActionFunction in_action_func)
	{
		return CheckAndProcess(

			in_input,
			in_title,
			true,
			in_action_func

		);
	}

	template<InputType INPUT_TYPE>
	bool InputActionEnumerator::CheckAndProcess(INPUT_TYPE in_input, char const* in_title, bool in_enabled, InputActionFunction in_action_func)
	{
		return CheckAndProcess(
		
			JustActivated(in_input),
			in_title,
			in_enabled, 
			in_action_func
		);
	}
#endif

#endif

}; // namespace chaos