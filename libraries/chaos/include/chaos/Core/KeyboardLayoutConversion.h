namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyboardLayoutConversion;
	class GLFWKeyScancodePair;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// GLFWKeyScancodePair: describe a relation between a GLFW keycode and a scancode. This is only valid for QWERTY while it is HARD-CODED inside GLFW
	class GLFWKeyScancodePair
	{
	public:

		/** the GLFW keycode */
		int keycode = 0;
		/** the corresponding scancode */
		unsigned int scancode = 0;
	};

	/**
	 * KeyboardLayoutConversion: convert key and scancode from/to qwerty/azerty and current layout
	 */

	class CHAOS_API KeyboardLayoutConversion
	{
	public:

		/** convert input key into the key at the same position */
		static Key ConvertToCurrentLayout(Key src);

	protected:

#if _WIN32 || _WIN64

		/** VK to GLFW keycode (relative to qwerty layout) */
		static int QwertyVKToGLFWKeycode(unsigned int vk);
		/** GLFW keycode to VK (relative to qwerty layout) */
		static unsigned int QwertyGLFWKeycodeToVK(int keycode);
		/** GLFW keycode to scancode (relative to qwerty layout) */
		static unsigned int QwertyGLFWKeycodeToScancode(int keycode);
		/** Scancode to GLFW keycode (relative to qwerty layout) */
		static int QwertyScancodeToGLFWKeycode(unsigned int scancode);

		/** get the GLFWKey/Scancode qwerty table */
		static std::vector<GLFWKeyScancodePair> const& GetQwertyGLFWKeyScancodeTable();

#endif // #if _WIN32 || _WIN64
	};

#endif

}; // namespace chaos