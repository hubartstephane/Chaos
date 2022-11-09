namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ButtonStateChange;

	template<typename T>
	class InputState;

	class ButtonState;
	class AxisState;

	class KeyEvent;
	class InputEventReceiverInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ButtonStateChange
	*/

	enum class CHAOS_API ButtonStateChange : int
	{
		/** unknown value */
		NONE = 0,
		/** button status change */
		STAY_RELEASED = 1,
		/** button status change */
		STAY_PRESSED = 2,
		/** button status change */
		BECOME_RELEASED = 3,
		/** button status change */
		BECOME_PRESSED = 4
	};

	/**
	* InputState
	*/

	template<typename T>
	class /*CHAOS_API*/ InputState
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
	* ButtonState
	*/

	class CHAOS_API ButtonState : public InputState<bool>
	{
	public:

		/** update the value */
		void SetValue(bool in_value);
		/** update the timer accumulator */
		void UpdateSameValueTimer(float delta_time);

		/** whether the button is pressed */
		bool IsPressed(bool previous_frame = false) const;
		/** whether the button has just been pressed */
		bool IsJustPressed() const;
		/** whether the button has just been released */
		bool IsJustReleased() const;

		/** get the button state change */
		ButtonStateChange GetStateChange() const;
	};

	/**
	* AxisState : while max and min values for sticks are not always 1 (some controllers has value lesser that 1.0),
	*            we have to store the upper and lower values to renormalize the output
	*/
	class CHAOS_API AxisState : public InputState<float>
	{
	public:

		/** update the value */
		void SetValue(float in_raw_value, float dead_zone);
		/** update the timer accumulator */
		void UpdateSameValueTimer(float delta_time);

	protected:

		/** min value always encountered */
		float min_value = -0.8f;
		/** max value always encountered */
		float max_value = +0.8f;
	};

	/**
	* KeyEvent
	*/

	class CHAOS_API KeyEvent
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
		bool CheckButtonPressed(Key button, bool previous_frame = false);
		/** check whether a key in a set is pressed */
		bool CheckButtonPressed(Key const* buttons, bool previous_frame = false);

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

		/** internal method to check whether a button is pressed */
		virtual bool DoCheckButtonPressed(Key button, bool previous_frame);

	protected:

		/** the current input mode */
		InputMode input_mode = InputMode::KEYBOARD;
	};

#endif

}; // namespace chaos