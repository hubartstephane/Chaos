namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class PollInputActionProcessor;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * PollInputActionProcessor: a specialization of InputActionProcessor dedicated to input polling
	 */

	class CHAOS_API PollInputActionProcessor : public InputActionProcessor
	{
	public:

		using InputActionProcessor::InputActionProcessor;

		/** override */
		virtual bool CheckAndProcess(InputConditionBase const& in_condition, char const* in_title, InputAction const & in_action) override;
	};

#endif

}; // namespace chaos