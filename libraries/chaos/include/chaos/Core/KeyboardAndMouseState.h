namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyboardAndMouseState;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyboardAndMouseState: A singleton that contains the state of keyboard and mouse buttons
	*/

	class CHAOS_API KeyboardAndMouseState : public Singleton<KeyboardAndMouseState>
	{

	public:

		/** get the state of a mouse or keyboard state */
		static ButtonState const * GetKeyState(Key key);
		/** change the state of a keyboard or mouse key (notification from a window) */
		static void SetKeyValue(Key key, bool value);

	protected:

		/** the state of all keyboard buttons */
		std::array<ButtonState, GLFW_KEY_LAST> keyboard_state;
		/** the state of all mouse buttons */
		std::array<ButtonState, GLFW_MOUSE_BUTTON_LAST + 1> mouse_button_state;
	};

#endif

}; // namespace chaos