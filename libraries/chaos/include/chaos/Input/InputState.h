namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InputStatus;

	template<typename T>
	class InputState;

	using KeyState     = InputState<bool>;
	using Input1DState = InputState<float>;
	using Input2DState = InputState<glm::vec2>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* InputStatus
	*/

	enum class CHAOS_API InputStatus : int
	{
		NONE = 0,
		STAY_INACTIVE = 1,
		STAY_ACTIVE = 2,
		BECOME_INACTIVE = 3,
		BECOME_ACTIVE = 4
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
		/** returns whether the input is activated */
		bool IsActive() const
		{
			return IsValueActive(value);
		}
		/** whether the input is not active */
		bool IsInactive() const
		{
			return !IsActive();
		}
		/** whether the input as just became active */
		bool IsJustActivated() const
		{
			return (IsActive()) && (GetSameValueTimer() == 0.0f);
		}
		/** whether the input as just became inactive */
		bool IsJustDeactivated() const
		{
			return (IsInactive()) && (GetSameValueTimer() == 0.0f);
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
		/** get the input status */
		InputStatus GetStatus() const
		{
			float same_value_time = GetSameValueTimer();

			if (IsActive())
			{
				if (same_value_time == 0.0f && update_time >= 0.0f)
					return InputStatus::BECOME_ACTIVE;
				else
					return InputStatus::STAY_ACTIVE;
			}
			else
			{
				if (same_value_time == 0.0f && update_time >= 0.0f)
					return InputStatus::BECOME_INACTIVE;
				else
					return InputStatus::STAY_INACTIVE;
			}
		}

		/** change the value of the input */
		void SetValue(T in_value)
		{
			double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();

			if (update_time < 0.0) // very first initialization
			{
				update_time = frame_time;
			}
			else if (IsValueActive(value) != IsValueActive(in_value)) // some effective change (because for float input, there always are some slight changes, we can't rely on strict comparison)
			{
				update_time = frame_time;
			}
			value = in_value;
		}

	protected:

		/** check whether a data is an active value */
		bool IsValueActive(T in_value) const
		{
			if constexpr (std::is_same_v<bool, T>)
				return in_value;
			if constexpr (std::is_same_v<float, T>)
				return (in_value != 0.0f);
			if constexpr (std::is_same_v<glm::vec2, T>)
				return (in_value.x != 0.0f) || (in_value.y != 0.0f);
			assert(0);
			return false;
		}

	public:

		/** value of the button (pressed or not) */
		T value = T();
		/** time when the state has been updated */
		double update_time = -1.0;
	};

	/**
	 * Some simple functions
	 */

	template<typename T>
	bool IsInputActive(InputState<T> const * state)
	{
		if (state == nullptr)
			return false;
		return state->IsActive();
	}

	template<typename T>
	bool IsInputInactive(InputState<T> const* state)
	{
		if (state == nullptr)
			return false;
		return state->IsInactive();
	}

	template<typename T>
	bool IsInputJustActivated(InputState<T> const* state)
	{
		if (state == nullptr)
			return false;
		return state->IsJustActivated();
	}

	template<typename T>
	bool IsInputJustDeactivated(InputState<T> const* state)
	{
		if (state == nullptr)
			return false;
		return state->IsJustDeactivated();
	}

#endif

}; // namespace chaos