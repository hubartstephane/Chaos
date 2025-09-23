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

		/** the operator */
		virtual bool CheckAndProcess(InputRequestBase const & in_request, char const * in_title, bool in_enabled, InputActionFunction in_action_func);

		/** lighter function, with in_enabled defaulted to true */
		bool CheckAndProcess(InputRequestBase const & in_request, char const * in_title, InputActionFunction in_action_func);
	};

#endif

}; // namespace chaos