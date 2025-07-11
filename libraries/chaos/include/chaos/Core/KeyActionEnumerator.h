namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyActionEnumerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyActionEnumerator
	*/

	class CHAOS_API KeyActionEnumerator
	{
		using KeyActionFunction = LightweightFunction<void()>;

	public:

		/** the operator */
		virtual bool operator () (KeyRequest const & in_request, char const * in_title, bool in_enabled, KeyActionFunction in_action_func);

		/** lighter function, with in_enabled defaulted to true */
		bool operator () (KeyRequest const & in_request, char const * in_title, KeyActionFunction in_action_func);
	};

#endif

}; // namespace chaos