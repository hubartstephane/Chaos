namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class KeyStatus;

	template<typename T>
	class InputState;

	class KeyState;
	class Input1DState;
	class Input2DState;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyStatus
	*/

	enum class CHAOS_API KeyStatus : int
	{
		NONE = 0,
		STAY_RELEASED = 1,
		STAY_PRESSED = 2,
		BECOME_RELEASED = 3,
		BECOME_PRESSED = 4
	};

	/**
	* InputState: base class for key/axis/stick state
	*/

	template<typename T>
	class InputState
	{
	public:

		/** get the value */
		T GetValue() const
		{
			return value;
		}
		/** get the timer for the same value */
		float GetSameValueTimer() const
		{
			if (update_time < 0.0)
				return 0.0f;
			return float(FrameTimeManager::GetInstance()->GetCurrentFrameTime() - update_time);
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

	protected:

		/** returns whether 2 input values should be considered as identical */
		static bool AreValuesSimilar(bool src1, bool src2)
		{
			return (src1 == src2);
		}
		/** returns whether 2 input values should be considered as identical */
		static bool AreValuesSimilar(float src1, float src2)
		{
			return (MathTools::GetExtendedSign(src1) == MathTools::GetExtendedSign(src2));
		}
		/** returns whether 2 input values should be considered as identical */
		static bool AreValuesSimilar(glm::vec2 const & src1, glm::vec2 const & src2)
		{
			float sqr_length_src1 = glm::length2(src1);
			float sqr_length_src2 = glm::length2(src2);
			return (MathTools::GetExtendedSign(sqr_length_src1) == MathTools::GetExtendedSign(sqr_length_src2));
		}
		/** internal method to set value and update internal time */
		void DoSetValue(T in_value)
		{
			double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();

			if (update_time < 0.0) // very first initialization
			{
				update_time = frame_time;
			}
			else if (!AreValuesSimilar(value, in_value)) // some change
			{
				update_time = frame_time;
			}
			value = in_value;
		}

	public:

		/** value of the button (pressed or not) */
		T value = T();
		/** time when the state has been updated */
		double update_time = -1.0;
	};

	/**
	* KeyState
	*/

	class CHAOS_API KeyState : public InputState<bool>
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
		KeyStatus GetStatus() const;
	};

	/**
	* Input1DState: An input state for 1D values (gamepad axes, mouse wheel ...)
	*/
	class CHAOS_API Input1DState : public InputState<float>
	{
	public:

		/** update the value */
		void SetValue(float in_value);
	};

	/**
	* Input2DState: An input state for 2D values (gamepad sticks, mouse movement ...)
	*/

	class CHAOS_API Input2DState : public InputState<glm::vec2>
	{
	public:

		/** update the value */
		void SetValue(glm::vec2 in_value);
	};

#endif

}; // namespace chaos