namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ButtonStateChange;
	enum class InputStateFrame;

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
		NONE = 0,
		STAY_RELEASED = 1,
		STAY_PRESSED = 2,
		BECOME_RELEASED = 3,
		BECOME_PRESSED = 4
	};

	/**
	* InputStateFrame
	*/

	enum class CHAOS_API InputStateFrame : int
	{
		CURRENT,
		PREVIOUS
	};

	/**
	* InputState
	*/

	template<typename T>
	class InputState
	{
	public:

		/** get the timer for the same value */
		float GetSameValueTimer() const
		{
			if (update_time < 0.0)
				return 0.0f;
			return float(FrameTimeManager::GetInstance()->GetCurrentFrameTime() - update_time);
		}
		/** get the value */
		T GetValue(InputStateFrame frame = InputStateFrame::CURRENT) const
		{
			return (frame == InputStateFrame::CURRENT) ? value : previous_value; // should work event if state has never been set (because initialized to 0)
		}
		/** returns whether the state has been set at least set */
		bool IsStateInitialized() const
		{
			return (update_time >= 0.0);
		}
		/** clear the input */
		void Clear()
		{
			value = previous_value = T();
			update_time = -1.0;
		}

	protected:

		/** value of the button (pressed or not) */
		T value = T();
		/** the previous value of the stick */
		T previous_value = T();
		/** time when the state has been updated */
		double update_time = -1.0;

	};

	/**
	* ButtonState
	*/

	class CHAOS_API ButtonState : public InputState<bool>
	{
	public:

		/** update the value */
		void SetValue(bool in_value);

		/** whether the button is pressed */
		bool IsPressed(InputStateFrame frame = InputStateFrame::CURRENT) const;
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

	protected:

		/** min value always encountered */
		float min_value = -0.8f;
		/** max value always encountered */
		float max_value = +0.8f;
	};

#endif

}; // namespace chaos