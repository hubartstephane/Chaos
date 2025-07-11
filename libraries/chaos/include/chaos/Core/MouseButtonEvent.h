namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class MouseButtonEvent;

	using MouseButtonRequest = KeyRequestBase<int>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* MouseButtonEvent: a mouse button event
	*/

	class CHAOS_API MouseButtonEvent : public KeyEventBase<int>
	{
	};

#endif

}; // namespace chaos