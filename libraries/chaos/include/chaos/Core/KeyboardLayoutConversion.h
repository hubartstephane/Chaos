namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyboardLayoutConversion;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * KeyboardLayoutConversion: convert key and scancode from/to qwerty/azerty and current layout
	 */

	class CHAOS_API KeyboardLayoutConversion
	{
	public:

		/** convert a virtual key, may return the entry that has been used for the conversion */
		static unsigned int ConvertVirtualKeyToCurrentLayout(unsigned int vk, KeyboardLayout layout, ScancodeInformation const ** result_pair);

		/** convert the scancode into GLFW keycode (QWERTY) */
		static int ScancodeToGLFWKey(int scancode);
	};

#endif

}; // namespace chaos