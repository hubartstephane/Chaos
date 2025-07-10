namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class KeyAction;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyAction: the state of the key
	*/

	enum class CHAOS_API KeyAction : int
	{
		Release = GLFW_RELEASE,
		Press = GLFW_PRESS,
		Repeat = GLFW_REPEAT
	};

#endif

}; // namespace chaos