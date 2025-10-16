namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyEvent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyEvent: a keyboard event
	*/

	class CHAOS_API KeyEvent : public KeyEventBase
	{

	public:

		/** the scan code */
		int scancode = 0;
	};

#endif

}; // namespace chaos