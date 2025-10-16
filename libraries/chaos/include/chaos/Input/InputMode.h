namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InputMode;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class CHAOS_API InputMode : int
	{
		/** the mode for keyboard */
		KEYBOARD = 0,
		/** the mode for Mouse */
		MOUSE = 1,
		/** the mode for Gamepad */
		GAMEPAD = 2
	};

	/** returns true whether given mode correspond to mouse or keyboard */
	CHAOS_API bool IsPCMode(InputMode mode);
	/** returns true whether given mode correspond to gamepad */
	CHAOS_API bool IsConsoleMode(InputMode mode);
	/** returns true whether we change platform (PC to console or inverse) */
	CHAOS_API bool IsPlatformChanged(InputMode new_mode, InputMode old_mode);

#endif

}; // namespace chaos