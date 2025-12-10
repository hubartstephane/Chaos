namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class KeyModifier;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyModifier: the keyboard modifier
	*/

	enum class CHAOS_API KeyModifier : int
	{
		None = 0,
		Shift = GLFW_MOD_SHIFT,
		Control = GLFW_MOD_CONTROL,
		Alt = GLFW_MOD_ALT
	};

	CHAOS_DECLARE_ENUM_METHOD(KeyModifier, CHAOS_API);
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(KeyModifier, CHAOS_API); // this is possible because GLFW_MOD_SHIFT, GLFW_MOD_CONTROL and GLFW_MOD_ALT are already a bit mask

#endif

}; // namespace chaos