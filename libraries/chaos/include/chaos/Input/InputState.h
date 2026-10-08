namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InputStatus;
	enum class InputStatusCheckType;

	template<typename T>
	class InputStateType;

	template<AnyInputType T>
	struct InputTypeToStateType : public boost::mpl::identity<
		InputStateType<
			InputTypeToValueType_t<T>
		>
	> {};

	template<AnyInputType T>
	using InputTypeToStateType_t = InputTypeToStateType<T>::type;

	using KeyState     = InputTypeToStateType_t<Key>;
	using Input1DState = InputTypeToStateType_t<Input1D>;
	using Input2DState = InputTypeToStateType_t<Input2D>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* InputStatus
	*/

	enum class InputStatus : int
	{
		None = 0,
		RepeatInactive = 1,
		RepeatActive = 2,
		JustActivated = 3,
		JustDeactivated = 4
	};

	/**
	* InputStatusCheckType: The requested input status
	*/

	enum class InputStatusCheckType : int
	{
		None,
		Inactive,
		JustDeactivated,
		InactiveRepeated,
		Active,
		JustActivated,
		ActiveRepeated,
	};

	CHAOS_DECLARE_ENUM_METHOD(InputStatusCheckType, CHAOS_API);


	/**
	* InputStateType: base class for key/axis/stick state
	*/

	template<typename T>
	class InputStateType
	{
		friend class InputDeviceInterface;

	public:

		using type = T;

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
			return GetStatus() == InputStatus::JustActivated;
		}
		/** whether the input as just became inactive */
		bool IsJustDeactivated() const
		{
			return GetStatus() == InputStatus::JustDeactivated;
		}
		/** returns whether the input is active and repeated */
		bool IsActiveRepeated() const
		{
			return GetStatus() == InputStatus::RepeatActive;
		}
		/** returns whether the input is inactive and repeated */
		bool IsInactiveRepeated() const
		{
			return GetStatus() == InputStatus::RepeatInactive;
		}
		/** clear the input */
		void Clear()
		{
			value          = {};
			previous_value = {};
			update_time    = -1.0;
		}
		/** get the input status */
		InputStatus GetStatus() const
		{
			double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();

			if (frame_time == update_time) // we can rely on previous_value
			{
				if (IsValueActive(value))
				{
					if (IsValueActive(previous_value))
						return InputStatus::RepeatActive;
					else
						return InputStatus::JustActivated;
				}
				else
				{
					if (IsValueActive(previous_value))
						return InputStatus::JustDeactivated;
					else
						return InputStatus::RepeatInactive;
				}
			}
			else // value is already an old value. it represents both current value and previous_value (value has not changed over time)
			{
				if (IsValueActive(value))
					return InputStatus::RepeatActive;
				else
					return InputStatus::RepeatInactive;
			}
		}

		/** change the value of the input */
		void SetValue(type in_value)
		{
			double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();
			if (frame_time == update_time)
				return;

			previous_value = value;
			value          = in_value;
			update_time    = frame_time;
		}

		/** a generic check function */
		bool CheckStatus(InputStatusCheckType check_type) const
		{
			switch (check_type)
			{
			case InputStatusCheckType::None:
				return true;
			case InputStatusCheckType::Inactive:
				return IsInactive();
			case InputStatusCheckType::JustDeactivated:
				return IsJustDeactivated();
			case InputStatusCheckType::InactiveRepeated:
				return IsInactiveRepeated();
			case InputStatusCheckType::Active:
				return IsActive();
			case InputStatusCheckType::JustActivated:
				return IsJustActivated();
			case InputStatusCheckType::ActiveRepeated:
				return IsActiveRepeated();
			default:
				assert(0);
			}
			return true;
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

		/** value of the input */
		type value = {};
		/** value of the input during last update */
		type previous_value = {};

	protected:

		/** time when the state has been updated */
		double update_time = -1.0;
	};













	// will be removed when refactor is over


	/**
	 * Standalone functions
	 */

	template<typename T>
	bool IsInputActive(InputStateType<T> const& state)
	{
		return state.IsActive();
	}

	template<typename T>
	bool IsInputInactive(InputStateType<T> const& state)
	{
		return state.IsInactive();
	}

	template<typename T>
	bool IsInputJustActivated(InputStateType<T> const& state)
	{
		return state.IsJustActivated();
	}

	template<typename T>
	bool IsInputJustDeactivated(InputStateType<T> const& state)
	{
		return state.IsJustDeactivated();
	}

	template<typename T>
	bool IsInputActiveRepeated(InputStateType<T> const& state)
	{
		return state.IsActiveRepeated();
	}

	template<typename T>
	bool IsInputInactiveRepeated(InputStateType<T> const& state)
	{
		return state.IsInactiveRepeated();
	}

	template<typename T>
	bool IsInputActive(std::optional<InputStateType<T>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputActive(state.value());
	}

	template<typename T>
	bool IsInputInactive(std::optional<InputStateType<T>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputInactive(state.value());
	}

	template<typename T>
	bool IsInputJustActivated(std::optional<InputStateType<T>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputJustActivated(state.value());
	}

	template<typename T>
	bool IsInputJustDeactivated(std::optional<InputStateType<T>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputJustDeactivated(state.value());
	}

	template<typename T>
	bool IsInputActiveRepeated(std::optional<InputStateType<T>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputActiveRepeated(state.value());
	}

	template<typename T>
	bool IsInputInactiveRepeated(std::optional<InputStateType<T>> const & state)
	{
		if (!state.has_value())
			return false;
		return IsInputInactiveRepeated(state.value());
	}

#endif

}; // namespace chaos