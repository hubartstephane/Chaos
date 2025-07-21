namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyboardAndMouseState;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyboardAndMouseState: A singleton that contains the state of keyboard and mouse buttons
	*/

	class CHAOS_API KeyboardAndMouseState : public Singleton<KeyboardAndMouseState>, public InputDeviceUserInterface
	{

	public:

		/** override */
		virtual KeyState const * GetKeyState(Key key) const override;
		/** override */
		virtual bool ForAllKeys(LightweightFunction<bool(Key, KeyState const &)> func) const override;

		/** change the state of a keyboard or mouse key (notification from a window) */
		void SetKeyValue(Key key, bool value);

	protected:

		/** the state of all keyboard buttons */
		std::array<KeyState, GLFW_KEY_LAST> keyboard_state;
		/** the state of all mouse buttons */
		std::array<KeyState, GLFW_MOUSE_BUTTON_LAST + 1> mouse_button_state;
	};

#endif

}; // namespace chaos