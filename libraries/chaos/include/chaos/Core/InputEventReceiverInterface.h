namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyEvent;
	class InputEventReceiverInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyEvent
	*/

	class CHAOS_API KeyEvent
	{
	public:

		/** check whether the key event is for considered key */
		bool IsKeyEvent(int check_keycode, int check_modifier = 0) const;

		/** check whether this is a key press event (no key requirement) */
		bool IsKeyPressed() const;
		/** check whether this is a key press event */
		bool IsKeyPressed(int check_keycode, int check_modifier = 0) const;

		/** check whether this is a key release event (no key requirement) */
		bool IsKeyReleased() const;
		/** check whether this is a key release event */
		bool IsKeyReleased(int check_keycode, int check_modifier = 0) const;

	public:

		/** the keycode (GLFW code) */
		int keycode = 0;
		/** the scan code */
		int scancode = 0;
		/** pressed or release */
		int action = 0;
		/** some special key modifiers like shift */
		int modifier = 0;
	};

	/**
	* InputEventReceiverInterface
	*/

	class CHAOS_API InputEventReceiverInterface
	{

	public:

		/** destructor */
		virtual ~InputEventReceiverInterface() = default;

		/** change the current input mode */
		void SetInputMode(InputMode new_mode);
		/* get the current input mode */
		InputMode GetInputMode() const { return input_mode; }

		/** check whether a key is pressed. Change input mode according to success */
		bool CheckKeyPressed(Key key, bool previous_frame = false);
		/** check whether a key in a set is pressed */
		bool CheckKeyPressed(Key const* keys, bool previous_frame = false);

		/** called whenever the mouse is moved */
		bool OnMouseMove(glm::vec2 const & delta);
		/** called whenever the mouse button is down / up */
		bool OnMouseButton(int button, int action, int modifier);
		/** called whenever the mouse wheel is changed */
		bool OnMouseWheel(double scroll_x, double scroll_y);
		/** called whenever a key is pressed */
		bool OnKeyEvent(KeyEvent const& event);
		/** called whenever a char is generated */
		bool OnCharEvent(unsigned int c);

	protected:

		/** called whenever the mouse is moved */
		virtual bool OnMouseMoveImpl(glm::vec2 const& delta);
		/** called whenever the mouse button is down / up */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier);
		/** called whenever the mouse wheel is changed */
		virtual bool OnMouseWheelImpl(double scroll_x, double scroll_y);
		/** called whenever a key is pressed */
		virtual bool OnKeyEventImpl(KeyEvent const& event);
		/** called whenever a char is generated */
		virtual bool OnCharEventImpl(unsigned int c);

		/** the user callback called when current input mode changes */
		virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode);

		/** internal method to check whether a button is pressed */
		virtual bool DoCheckKeyPressed(Key key, bool previous_frame);

	protected:

		/** the current input mode */
		InputMode input_mode = InputMode::KEYBOARD;
	};

#endif

}; // namespace chaos