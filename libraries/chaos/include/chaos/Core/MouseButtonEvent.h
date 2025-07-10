namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class MouseButtonEvent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* MouseButtonEvent: a mouse button event
	*/

	class CHAOS_API MouseButtonEvent : public KeyEventBase<int, 0>
	{
	};

#endif

}; // namespace chaos