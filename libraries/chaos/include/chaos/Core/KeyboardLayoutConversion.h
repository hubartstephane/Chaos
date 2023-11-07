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
		int glfw_keycode = 0;
		/** the corresponding scancode */
		unsigned int scancode = 0;
	};

	/**
	 * KeyboardLayoutConversion: convert key and scancode from/to qwerty/azerty and current layout
	 */

	 // GLFW keycodes represent the 'character' that would be produced on a qwerty keyboard -+
	 //                                                                                      +--> GLFW keycodes and VK represent CHARACTER (there is a link to be made between this 2 concepts)
	 // VK (virtual key) represent a 'character' on any keyboard                            -+
	 // 
	 // Scancodes represent a position on the keyboard (independant of the character produced) -> SCANCODE only represents a position
	 //
	 // 
	 // With GLFW, you can get the qwerty scancode corresponding to GLFW keycode
	 // With Win32 API, you can make a relation between scancode and VK
	 //   -> Using both information, you can make a translation GLFW keycode <--> VK
	 // 
	 //                 ----------------------------------------------------------------------------
	 //
	 // GLFW_KEY_A / QWERTY: the key at the same position than the key that would produce a 'A' on QWERTY layout
	 //                      a conversion is required to use it properly:
	 // 
	 //                      -search the scancode on QWERTY keyboard
	 //                      -search the VK for this scancode on current keyboard
	 //                      -search the GLFW keycode corresponding to this VK
	 // 
	 //                      GLFW --> SCANCODE (qwerty layout) --> VK (current layout) --> GLFW
	 // 
	 // 
	 //                 ----------------------------------------------------------------------------
	 // 
	 // 
	 // GLFW_KEY_A / AZERTY: the key at the same position than the key that would produce a 'A' on AZERTY layout
	 //                      a conversion is required to use it properly:
	 //
	 //                      -search the VK corresponding to GLFW_KEY_A
	 //                      -search the scancode for this VK in AZERTY layout
	 //                      -search the GLFW keycode corresponding to this scancode
	 //
	 //                      GLFW --> VK (qwerty layout) --> SCANCODE (azerty layout) --> GLFW
	 //
	 // GLFW_KEY_A / CURRENT: the key that display a 'A' on current keyboard
	 //                       a conversion is required to use it properly (GLFW_KEY_A is the key that would produce a 'A' if the layout was QWERTY)
	 // 
	 //                       -search VK corresponding to GLFW_KEY_A
	 //                       -search the scancode for this VK in CURRENT layout
	 //                       -search the GLFW keycode corresponding to this scancode (for QWERTY layout)
	 // 
	 //                       GLFW --> VK (qwerty layout) --> SCANCODE (current layout) --> GLFW

	class CHAOS_API KeyboardLayoutConversion
	{


	public:

		/** convert VK from one layout to another */
		static unsigned int ConvertVK(unsigned int vk, KeyboardLayoutType src_layout_type = KeyboardLayoutType::QWERTY, KeyboardLayoutType dst_layout_type = KeyboardLayoutType::CURRENT);
		/** convert a GLFW keycode from a layout to another layout (into a key that has the same position) */
		static int ConvertGLFWKeycode(int keycode, KeyboardLayoutType src_layout_type, KeyboardLayoutType dst_layout_type);

	protected:

#if _WIN32

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

#endif // #if _WIN32



#if 0






	public:

		/** convert a key by position */
		static Key ConvertKey(Key key, KeyboardLayoutType layout);
		/** convert VK from one layout to another */
		static unsigned int ConvertVK(unsigned int vk, KeyboardLayoutType src_layout_type = KeyboardLayoutType::QWERTY, KeyboardLayoutType dst_layout_type = KeyboardLayoutType::CURRENT);
		/** convert input button */
		static KeyboardButton ConvertKeyboardButton(KeyboardButton button, KeyboardLayoutType layout);
		/** convert a GLFW keycode from a layout to another layout (into a key that has the same position) */
		static int ConvertGLFWKeycode(int keycode, KeyboardLayoutType src_layout_type, KeyboardLayoutType dst_layout_type);

	protected:

#if _WIN32

		/** VK to GLFW keycode (relative to qwerty layout) */
		static KeyboardButton QwertyVKToGLFWKeycode(unsigned int vk);
		/** GLFW keycode to VK (relative to qwerty layout) */
		static unsigned int QwertyGLFWKeycodeToVK(KeyboardButton keycode);
		/** GLFW keycode to scancode (relative to qwerty layout) */
		static unsigned int QwertyGLFWKeycodeToScancode(KeyboardButton keycode);
		/** Scancode to GLFW keycode (relative to qwerty layout) */
		static KeyboardButton QwertyScancodeToGLFWKeycode(unsigned int scancode);

		/** get the GLFWKey/Scancode qwerty table */
		static std::vector<GLFWKeyScancodePair> const& GetQwertyGLFWKeyScancodeTable();

#endif // #if _WIN32

#endif


	};

#endif

}; // namespace chaos