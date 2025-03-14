namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyboardState;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyboardState
	*/

	class CHAOS_API KeyboardState
	{
		friend class Window;
		friend class WindowApplication;

	public:

		/** get the state of a mouse or keyboard state */
		static ButtonState const * GetKeyState(Key key);
		/** get the state of a keyboard key */
		static ButtonState const* GetKeyboardButtonState(KeyboardButton key);
		/** get the state of a mouse key */
		static ButtonState const* GetMouseButtonState(MouseButton key);

	protected:

		/** change the state of a keyboard key (notification from a window)*/
		static void SetKeyboardButtonState(KeyboardButton key, int action);
		/** change the state of a mouse key (notification from a window)*/
		static void SetMouseButtonState(MouseButton key, int action);
		/** update the internal timers of keyboard states */
		static void UpdateKeyStates(float delta_time);

	protected:

		/** the state of all buttons */
		static std::array<ButtonState, GLFW_KEY_LAST> keyboard_state;
		/** the state of all buttons */
		static std::array<ButtonState, GLFW_MOUSE_BUTTON_LAST + 1> mouse_button_state;
	};

#endif

}; // namespace chaos