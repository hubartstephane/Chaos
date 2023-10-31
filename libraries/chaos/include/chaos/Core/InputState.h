namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ButtonStateChange;

	template<typename T>
	class InputState;

	class ButtonState;
	class AxisState;

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
		void UpdateTimerAccumulation(float delta_time);

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
		void UpdateTimerAccumulation(float delta_time);

	protected:

		/** min value always encountered */
		float min_value = -0.8f;
		/** max value always encountered */
		float max_value = +0.8f;
	};

#endif

}; // namespace chaos