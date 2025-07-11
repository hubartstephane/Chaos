namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class KeyAction;
	enum class KeyActionMask;

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

	/**
	* KeyActionMask: a mask of wanted action 
	*/

	enum class CHAOS_API KeyActionMask : int
	{
		None    = 0,
		Release = 1,
		Press   = 2,
		Repeat  = 4,
		Down    = Press | Repeat
	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(KeyActionMask, CHAOS_API);

#endif

}; // namespace chaos