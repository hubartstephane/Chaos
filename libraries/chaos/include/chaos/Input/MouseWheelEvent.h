namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class MouseWheelEvent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* MouseWheelEvent: a mouse wheel event
	*/

	class CHAOS_API MouseWheelEvent
	{

	public:

		/** the scroll value */
		glm::vec2 scroll = {0.0f, 0.0f};
	};

#endif

}; // namespace chaos