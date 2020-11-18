#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	template<typename T>
	class InputState;

	class KeyEvent;
	class InputEventReceiver;

}; // namespace chaos

#else 

namespace chaos
{

	/**
	* InputState
	*/

	template<typename T>
	class InputState
	{
	public:

		/** get the timer for the same value */
		float GetSameValueTimer() const { return same_value_timer; }
		/** get the value */
		T GetValue(bool previous_frame = false) const
		{
			return (previous_frame) ? previous_value : value;
		}
		/** clear the input */
		void Clear()
		{
			value = previous_value = T();
			same_value_timer = 0.0f;
		}

	protected:

		/** value of the button (pressed or not) */
		T value = T();
		/** the previous value of the stick */
		T previous_value = T();
		/** the duration for which the value is the same */
		float same_value_timer = 0.0f;
	};

	/**
	* KeyEvent
	*/

	class KeyEvent
	{
	public:

		/** check whether the key event is for considered key */
		bool IsKeyEvent(int check_key, int check_modifier = 0) const;

		/** check whether this is a key press event (no key requirement) */
		bool IsKeyPressed() const;
		/** check whether this is a key press event */
		bool IsKeyPressed(int check_key, int check_modifier = 0) const;

		/** check whether this is a key release event (no key requirement) */
		bool IsKeyReleased() const;
		/** check whether this is a key release event */
		bool IsKeyReleased(int check_key, int check_modifier = 0) const;

	public:

		/** the key */
		int key = 0;
		/** the scan code */
		int scan_code = 0;
		/** pressed or release */
		int action = 0;
		/** some special key modifiers like shift */
		int modifier = 0;
	};

	/**
	* InputEventReceiver
	*/

	class InputEventReceiver
	{

	public:

		/** destructor */
		virtual ~InputEventReceiver() = default;

		/** change the current input mode */
		void SetInputMode(InputMode new_mode);
		/* get the current input mode */
		InputMode GetInputMode() const { return input_mode; }
		
		/** check whether a key event correspond to given request. Change input mode to keyboard if true */
		bool CheckKeyPressed(KeyEvent const& event, int check_key, int check_modifier = 0);
		/** check whether a key is pressed given request. Change input mode to keyboard if true */
		bool CheckKeyPressed(int check_key);

		/** called whenever the mouse is moved */
		bool OnMouseMove(double x, double y);
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
		virtual bool OnMouseMoveImpl(double x, double y);
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

	protected:

		/** the current input mode */
		InputMode input_mode = InputMode::KEYBOARD;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



