namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InputMode;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class InputMode : int
	{
		Keyboard = 0,
		Mouse = 1,
		Gamepad = 2
	};

	/** returns true whether given mode correspond to mouse or keyboard */
	CHAOS_API bool IsPCMode(InputMode mode);
	/** returns true whether given mode correspond to gamepad */
	CHAOS_API bool IsConsoleMode(InputMode mode);
	/** returns true whether we change platform (PC to console or inverse) */
	CHAOS_API bool IsPlatformChanged(InputMode new_mode, InputMode old_mode);

#endif

}; // namespace chaos