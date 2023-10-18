namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class KeyboardLayout;
	class KeyboardLayoutConversion;
	class KeyboardVirtualKeyConversionEntry;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// Note on keys:
	//
	//   WINDOW has some kind of code to describe a key:   VK_SPACE, VK_UP ... for example
	//   GLFW has another kind of code to describe a key:  GLFW_KEY_SPACE, GLFW_KEY_UP ... for example
	// 
	//   thoses constants don't match
	//
	//   the scancode is a code that describes the position of a key on the keyboard. This is the value of interest if we want to convert layouts
	//
	//   it is possible to make associations:
	//
	//     - from WINDOW's keycode to scancode        -> window_keycode:   window_keycode  = MapVirtualKey(scancode, MAPVK_VSC_TO_VK);
	//                                window_keycode  -> scancode:         scancode        = MapVirtualKey(window_keycode, MAPVK_VK_TO_VSC);
	//
	//     - from GLFW's keycode to scancode:    scancode = glfwGetKeyScancode(glfw_keycode);
	//       (there is no inverse conversion)
	// 
	//                                           beware: this scancode is the one you get from a QWERTY layout, whatever the current layout is
	//                                                   GLFW uses HARD-CODED values
	//
	//   On windows, there is a function that gives the window_keycode from a character:
	//
	//     window_keycode = ::VkKeyScan('$') & 0xFF         (the & 0xFF is important)
	//
	//  you can get the 'name' of a key by its scancode
	// 
	//    char name[256];
	//    ::GetKeyNameTextA(scancode << 16, name, sizeof(name));    (the << 16 is important)
	//
	//  Notes:
	//
	//    -WINDOWS (virtual) keycodes are in [0x00 - 0xFF]
	// 
	//    -GLFW's keycodes range is much bigger [0 .. 348]
	//
	//    -It is very likely that you don't really have an enum value for all keys of your keyboard 
	//     For example, there is no VK_ASTERISK nor GLFW_KEY_ASTERISK
	//     this is because on QWERTY (the base layout for thoses enums), there is no asterisk key of its own (on QWERTY, '*' in over the key '8')
	//    
	
	/**
	 * KeyboardLayout: known layout
	 */

	enum class KeyboardLayout
	{
		AZERTY,
		QWERTY
	};

	/**
	 * VirtualKeyScancodePair: information relation between scancode and virtual key
	 */

	class VirtualKeyScancodePair
	{
	public:

		/** the virtual key of the entry */
		unsigned int vk = 0;
		/** the scancode of the entry */
		unsigned int scancode = 0;
		/** the name of the key */
		std::string name;
	};

	/**
	 * KeyboardLayoutConversion: convert key and scancode from/to qwerty/azerty and current layout
	 **/

	class KeyboardLayoutConversion
	{
	public:

#if _WIN32 || _WIN64

		/** convert a virtual key, may return the entry that has been used for the conversion */
		static int ConvertVirtualKey(int src_vk, KeyboardLayout src_layout, VirtualKeyScancodePair const ** src_entry);

		/** get the VirtualKeyScancodePair table for the current layout */
		static std::vector<VirtualKeyScancodePair> GetVirtualKeyScancodeTable();

		/** get the name of a key corresponding to a scancode */
		static std::string ScancodeToName(int scancode);

#endif // #if _WIN32 || _WIN64



	};

#endif

}; // namespace chaos