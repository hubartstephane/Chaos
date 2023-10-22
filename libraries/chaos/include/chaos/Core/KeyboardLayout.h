namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class KeyboardLayoutType;
	class KeyboardLayout;
	class ScancodeInformation;

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
	//    -WINDOWS (virtual) keycodes are in [0x00 .. 0xFF]
	// 
	//    -GLFW's keycodes range is much bigger [0 .. 348]
	//
	//    -It is very likely that you don't really have an enum value for all keys of your keyboard 
	//     For example, there is no VK_ASTERISK nor GLFW_KEY_ASTERISK
	//     this is because on QWERTY (the base layout for thoses enums), there is no asterisk key of its own (on QWERTY, '*' in over the key '8')
	//
	//    - For WINDOWS, there is no VK_A .. VK_Z. used 'A' or 'Z' directly
	//                   there is no VK_0 .. VK_9. used '0' or '9' directly

	/**
	 * KeyboardLayoutType: known layout
	 */

	enum class CHAOS_API KeyboardLayoutType
	{
		AZERTY,
		QWERTY
	};

	/**
	 * ScancodeInformation: information relation between scancode and virtual key
	 */

	class CHAOS_API ScancodeInformation
	{
	public:

		/** the scancode of the entry */
		unsigned int scancode = 0;
		/** the virtual key of the entry */
		unsigned int vk = 0;
		/** the name of the key */
		std::string name;
	};

	/**
	 * KeyboardLayout
	 */

	class CHAOS_API KeyboardLayout
	{
	public:

		/** collect the current keyboard layout information */
		static KeyboardLayout Collect(bool capture_names_from_known_layout = true);
		/** get the information for a known layout */
		static KeyboardLayout const& GetKnownLayout(KeyboardLayoutType type);
		/** get scancode name */
		static std::string ScancodeToName(unsigned int scancode);

		/** get information from the scancode */
		ScancodeInformation const* GetInformationFromScancode(unsigned int scancode) const;
		/** get information from the virtual key */
		ScancodeInformation const* GetInformationFromVK(unsigned int vk) const;

		/** get the cached current layout */
		static KeyboardLayout const& GetCurrentLayout();
		/** invalidate the cached current layout */
		static void InvalidateCachedLayout();

	protected:

		/** returns a reference to the cached layout */
		static std::optional<KeyboardLayout>& GetCachedLayout();

	public:

		/** the informations for each keys */
		std::vector<ScancodeInformation> key_info;
	};

	/** this method is used to dump KeyboardLayout as a C++ code */
#if _DEBUG
	bool DumpKeyboardLayoutToFile(char const* filename, char const* table_name, KeyboardLayout const& information);
#endif

#endif

}; // namespace chaos