namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyEvent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyEvent: a keyboard event
	*/

	class CHAOS_API KeyEvent
	{
	public:

		/** check whether this is a key press event */
		bool IsKeyPressedEvent() const;
		/** check whether this is a key repeat event */
		bool IsKeyRepeatEvent() const;
		/** check whether this is a key release event */
		bool IsKeyReleasedEvent() const;
		/** check whether this is a key press or repeat event */
		bool IsKeyDownEvent() const;

		/** check whether this is a key press or repeat event */
		bool IsKeyDown(KeyboardButton check_button, int check_modifier = 0) const;
		/** check whether this is a key press event */
		bool IsKeyPressed(KeyboardButton check_button, int check_modifier = 0) const;
		/** check whether this is a key release event */
		bool IsKeyReleased(KeyboardButton check_button, int check_modifier = 0) const;
		/** check whether this is a key repeat event */
		bool IsKeyRepeat(KeyboardButton check_button, int check_modifier = 0) const;

	protected:

		/** check whether the key event is for considered key */
		bool IsKeyEvent(KeyboardButton check_button, int check_modifier = 0) const;

	public:

		/** the keycode */
		KeyboardButton button = KeyboardButton::UNKNOWN;
		/** the scan code */
		int scancode = 0;
		/** pressed or release */
		int action = 0;
		/** some special key modifiers like shift */
		int modifier = 0;
	};

#endif

}; // namespace chaos