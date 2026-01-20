namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InputStatus;

	template<InputType INPUT_TYPE>
	class InputStateBase;

	CHAOS_GENERATE_CLASS_MAPPING_DECLARATION(InputState, typename)
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputState, Key, InputStateBase<Key>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputState, Input1D, InputStateBase<Input1D>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputState, Input2D, InputStateBase<Input2D>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputState, MappedInput1D, InputStateBase<Input1D>);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputState, MappedInput2D, InputStateBase<Input2D>);

	using KeyState     = InputState_t<Key>;
	using Input1DState = InputState_t<Input1D>;
	using Input2DState = InputState_t<Input2D>;

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
	* InputStateBase: base class for key/axis/stick state
	*/

	template<InputType INPUT_TYPE>
	class InputStateBase
	{
	public:

		using type = InputValueType_t<INPUT_TYPE>;

		/** get the value */
		type GetValue() const
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
			return GetStatus() == InputStatus::BECOME_ACTIVE;
		}
		/** whether the input as just became inactive */
		bool IsJustDeactivated() const
		{
			return GetStatus() == InputStatus::BECOME_INACTIVE;
		}
		/** returns whether the input is active and repeated */
		bool IsActiveRepeated() const
		{
			return GetStatus() == InputStatus::STAY_ACTIVE;
		}
		/** returns whether the input is inactive and repeated */
		bool IsInactiveRepeated() const
		{
			return GetStatus() == InputStatus::STAY_INACTIVE;
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
			value = type();
			update_time = -1.0;
		}
		/** get the input status */
		InputStatus GetStatus() const
		{
			float same_value_time = GetSameValueTimer();
			bool  initialized     = IsStateInitialized();

			if (IsValueActive(value))
			{
				if (same_value_time == 0.0f && initialized)
					return InputStatus::BECOME_ACTIVE;
				else
					return InputStatus::STAY_ACTIVE;
			}
			else
			{
				if (same_value_time == 0.0f && initialized)
					return InputStatus::BECOME_INACTIVE;
				else
					return InputStatus::STAY_INACTIVE;
			}
		}

		/** change the value of the input */
		void SetValue(type in_value)
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
		static bool IsValueActive(type in_value)
		{
			if constexpr (std::is_same_v<bool, type>)
				return in_value;
			if constexpr (std::is_same_v<float, type>)
				return (in_value != 0.0f);
			if constexpr (std::is_same_v<glm::vec2, type>)
				return (in_value.x != 0.0f) || (in_value.y != 0.0f);
			assert(0);
			return false;
		}

	public:

		/** value of the button (pressed or not) */
		type value = {};
		/** time when the state has been updated */
		double update_time = -1.0;
	};

	/**
	 * Standalone functions
	 */

	template<InputType INPUT_TYPE>
	bool IsInputActive(InputStateBase<INPUT_TYPE> const& state)
	{
		return state.IsActive();
	}

	template<InputType INPUT_TYPE>
	bool IsInputInactive(InputStateBase<INPUT_TYPE> const& state)
	{
		return state.IsInactive();
	}

	template<InputType INPUT_TYPE>
	bool IsInputJustActivated(InputStateBase<INPUT_TYPE> const& state)
	{
		return state.IsJustActivated();
	}

	template<InputType INPUT_TYPE>
	bool IsInputJustDeactivated(InputStateBase<INPUT_TYPE> const& state)
	{
		return state.IsJustDeactivated();
	}

	template<InputType INPUT_TYPE>
	bool IsInputActiveRepeated(InputStateBase<INPUT_TYPE> const& state)
	{
		return state.IsActiveRepeated();
	}

	template<InputType INPUT_TYPE>
	bool IsInputInactiveRepeated(InputStateBase<INPUT_TYPE> const& state)
	{
		return state.IsInactiveRepeated();
	}

	template<InputType INPUT_TYPE>
	bool IsInputActive(std::optional<InputStateBase<INPUT_TYPE>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputActive(state.value());
	}

	template<InputType INPUT_TYPE>
	bool IsInputInactive(std::optional<InputStateBase<INPUT_TYPE>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputInactive(state.value());
	}

	template<InputType INPUT_TYPE>
	bool IsInputJustActivated(std::optional<InputStateBase<INPUT_TYPE>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputJustActivated(state.value());
	}

	template<InputType INPUT_TYPE>
	bool IsInputJustDeactivated(std::optional<InputStateBase<INPUT_TYPE>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputJustDeactivated(state.value());
	}

	template<InputType INPUT_TYPE>
	bool IsInputActiveRepeated(std::optional<InputStateBase<INPUT_TYPE>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputActiveRepeated(state.value());
	}

	template<InputType INPUT_TYPE>
	bool IsInputInactiveRepeated(std::optional<InputStateBase<INPUT_TYPE>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputInactiveRepeated(state.value());
	}

#endif

}; // namespace chaos