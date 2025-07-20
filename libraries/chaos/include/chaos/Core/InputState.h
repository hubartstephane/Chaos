namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ButtonStateChange;

	template<typename T>
	class InputState;

	class ButtonState;
	class AxisState;
	class StickState;

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
	* InputState: base class for button and axis state
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
		T GetValue() const
		{
			return value;
		}
		/** returns whether the state has been set at least set */
		bool IsStateInitialized() const
		{
			return (update_time >= 0.0);
		}
		/** clear the input */
		void Clear()
		{
			value = T();
			update_time = -1.0;
		}

	public:

		/** value of the button (pressed or not) */
		T value = T();
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

		/** whether the button is up (press or repeat) */
		bool IsDown() const;
		/** whether the button is up (released) */
		bool IsUp() const;
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

	public:

		/** min value always encountered */
		float min_value = -0.8f;
		/** max value always encountered */
		float max_value = +0.8f;
	};



	class CHAOS_API StickState : public InputState<glm::vec2>
	{

	public:

		/** min value always encountered */
		glm::vec2 min_value = {-0.8f, -0.8f};
		/** max value always encountered */
		glm::vec2 max_value = {+0.8f, +0.8f};
	};

#endif

}; // namespace chaos