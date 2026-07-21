namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class MouseMoveEvent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* MouseMoveEvent: a mouse move event
	*/

	class CHAOS_API MouseMoveEvent
	{

	public:

		/** the move value */
		glm::vec2 delta = {0.0f, 0.0f};
	};

#endif

}; // namespace chaos