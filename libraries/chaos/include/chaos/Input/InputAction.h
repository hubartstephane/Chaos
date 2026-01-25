namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

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
		InputAction() = default;
		/** copy constructor */
		InputAction(InputAction const&) = default;

		/** initializer constructor */
		template<std::invocable CALLABLE>
		InputAction(CALLABLE const & in_action_function, bool in_enabled = true):
			action_function(in_action_function),
			enabled(in_enabled)
		{}

		/** processing function */
		void Process() const;

	public:

		/** the function to execute */
		InputActionFunction action_function;
		/** whether the action is to be really done */
		bool enabled = true;
	};

#endif

}; // namespace chaos