namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputDeviceInterface;

	enum class InputDeviceTypeMask;

	template<typename T>
	class InputDeviceQueryResult;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Some aliases
	*/

	using ForAllKeysFunction = LightweightFunction<bool(Key, KeyState const &)>;

	using ForAllInput1DFunction = LightweightFunction<bool(Input1D, Input1DState const &)>;

	using ForAllInput2DFunction = LightweightFunction<bool(Input2D, Input2DState const &)>;

	using EnumerateDeviceHierarchyFunction = LightweightFunction<bool(InputDeviceInterface const*)>;

	/**
	* InputDeviceTypeMask: result when querying for an input
	*/

	enum class InputDeviceTypeMask : int
	{
		None = 0,
		Keyboard = 1,
		Mouse = 2,
		Gamepad = 4
	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(InputDeviceTypeMask, CHAOS_API);

	/**
	* InputDeviceQueryResult: result when querying for an input
	*/

	template<typename T>
	class InputDeviceQueryResult
	{
	public:

		using type = T;

		/** the state of the requested input */
		std::optional<InputState<type>> state;
		/** mask of all devices involved in the query */
		InputDeviceTypeMask involved_devices = InputDeviceTypeMask::None;
	};

	/**
	* InputDeviceInterface: a class to make request for some device state
	*/

	class CHAOS_API InputDeviceInterface
	{

	public:

		/** call a function on all devices handled by this whole hierarchy (composition pattern) */
		virtual bool EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const;

		/** gets any key state */
		InputDeviceQueryResult<bool> GetInputState(Key input) const;
		/** gets any input1D state */
		InputDeviceQueryResult<float> GetInputState(Input1D input) const;
		/** gets any input2D state */
		InputDeviceQueryResult<glm::vec2> GetInputState(Input2D input) const;
		/** gets any Mappedinput1D state */
		InputDeviceQueryResult<float> GetInputState(MappedInput1D const & input) const;
		/** gets any Mappedinput2D state */
		InputDeviceQueryResult<glm::vec2> GetInputState(MappedInput2D const & input) const;
		/** gets any CompositeInput state */
		template<typename... PARAMS>
		requires HaveSameInputValueType<PARAMS...>
		auto GetInputState(CombinedInput<PARAMS...> const & input) const;

		/** enumerate keys */
		bool ForAllKeys(ForAllKeysFunction func) const;
		/** enumerate input1D */
		bool ForAllInput1D(ForAllInput1DFunction func) const;
		/** enumerate input2D */
		bool ForAllInput2D(ForAllInput2DFunction func) const;

		/** get state value for any input */
		template<AnyInputType ANY_INPUT_TYPE>
		InputTypeToValueType_t<ANY_INPUT_TYPE> GetInputValue(ANY_INPUT_TYPE in_input) const
		{
			auto input_result = GetInputState(in_input);
			if (input_result.state.has_value())
				return input_result.state->GetValue();
			return {};
		}

		/** get state status for any input */
		template<AnyInputType ANY_INPUT_TYPE>
		InputStatus GetInputStatus(ANY_INPUT_TYPE in_input) const
		{
			auto input_result = GetInputState(in_input);
			if (input_result.state.has_value())
				return input_result.state->GetStatus();
			return InputStatus::None;
		}

		/** returns true whether there is any k active */
		bool IsAnyKeyActive() const;
		/** returns true whether there is any input1D active */
		bool IsAnyInput1DActive() const;
		/** returns true whether there is any input2D active */
		bool IsAnyInput2DActive() const;
		/** returns true whenever any key, input1D or input2D is active */
		bool IsAnyInputActive() const;

		/** returns true whether there is a key that just has become pressed */
		bool HasAnyKeyJustBecameActive() const;

	protected:

		/** utility method to get a state */
		template<BasicInputType BASIC_INPUT_TYPE>
		InputDeviceQueryResult<InputTypeToValueType_t<BASIC_INPUT_TYPE>> GetInputStateHelper(BASIC_INPUT_TYPE in_input) const;

		/** gets one key state */
		virtual InputDeviceQueryResult<bool> DoGetInputState(Key input) const;
		/** gets one input1D state */
		virtual InputDeviceQueryResult<float> DoGetInputState(Input1D input) const;
		/** gets one input2D state */
		virtual InputDeviceQueryResult<glm::vec2> DoGetInputState(Input2D input) const;

		/** enumerate keys */
		virtual bool DoForAllKeys(ForAllKeysFunction func) const;
		/** enumerate input1D */
		virtual bool DoForAllInput1D(ForAllInput1DFunction func) const;
		/** enumerate input2D */
		virtual bool DoForAllInput2D(ForAllInput2DFunction func) const;
	};

#else

template<typename... PARAMS>
requires HaveSameInputValueType<PARAMS...>
auto InputDeviceInterface::GetInputState(CombinedInput<PARAMS...> const& input) const
{
	double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();

	// for all child_input, check find the state, prepare optional parameter if necessary, and call user function
	auto CombineAllChildInputs = [&](auto & inout_result, auto const & combine_state_func)
	{
		auto CombineInputStates = [&](auto const & in_child_state)
		{
			if (!in_child_state.state.has_value())
				return;
			if (!inout_result.state.has_value())
			{
				inout_result.state.emplace();
				inout_result.state->update_time = frame_time;
			}
			if (in_child_state.state->IsActive())
				inout_result.involved_devices |= in_child_state.involved_devices;
			combine_state_func(inout_result.state.value(), in_child_state.state.value());
		};

		std::apply([&](auto const & ... child_input)
		{
			(CombineInputStates(GetInputState(child_input)), ...);

		}, input.child_inputs);
	};

	// if there is a POS and a NEG value, result is 0
	// elsewhere returns the direction which is not null
	auto MergeAxisValues = [](float min_neg_value, float max_pos_value)
	{
		if (max_pos_value == 0.0f && min_neg_value < 0.0f)
			return min_neg_value;
		if (max_pos_value > 0.0f && min_neg_value == 0.0f)
			return max_pos_value;
		return 0.0f;
	};

	// update MIN & MAX value according to VALUE sign
	auto UpdateMinAndMaxAxisValue = [](float value, float & min_neg_value, float & max_pos_value)
	{
		if (value > 0.0f)
			max_pos_value = std::max(max_pos_value, value);
		else if (value < 0.0f)
			min_neg_value = std::min(min_neg_value, value);
	};

	using input_value_type = CombinedInput<PARAMS...>::input_value_type;

	// KEY
	if constexpr (std::is_same_v<input_value_type, bool>)
	{
		InputDeviceQueryResult<bool> result;

		CombineAllChildInputs(result, [&](InputState<bool>& inout_state, InputState<bool> const& child_state)
		{
			bool child_previous_value = (child_state.update_time == frame_time)?
				child_state.previous_value :
				child_state.value;

			inout_state.value |= child_state.value;
			inout_state.previous_value |= child_previous_value; // final combined input result is TRUE if at least one child input is TRUE
		});

		return result;
	}
	// INPUT1D
	else if constexpr (std::is_same_v<input_value_type, float>)
	{
		InputDeviceQueryResult<float> result;

		float min_neg_value = 0.0f;
		float max_pos_value = 0.0f;
		float previous_min_neg_value = 0.0f;
		float previous_max_pos_value = 0.0f;

		// compute min & max value
		CombineAllChildInputs(result, [&](InputState<float>& inout_state, InputState<float> const& child_state)
		{
			float child_previous_value = (child_state.update_time == frame_time)?
				child_state.previous_value :
				child_state.value;

			UpdateMinAndMaxAxisValue(child_state.value, min_neg_value, max_pos_value);
			UpdateMinAndMaxAxisValue(child_previous_value, previous_min_neg_value, previous_max_pos_value);
		});

		if (result.state.has_value())
		{
			result.state->value = MergeAxisValues(min_neg_value, max_pos_value);
			result.state->previous_value = MergeAxisValues(previous_min_neg_value, previous_max_pos_value);
		}

		return result;
	}
	// INPUT2D
	else if constexpr (std::is_same_v<input_value_type, glm::vec2>)
	{
		InputDeviceQueryResult<glm::vec2> result;

		glm::vec2 min_neg_value = { 0.0f, 0.0f };
		glm::vec2 max_pos_value = { 0.0f, 0.0f };
		glm::vec2 previous_min_neg_value = { 0.0f, 0.0f };
		glm::vec2 previous_max_pos_value = { 0.0f, 0.0f };

		// compute min & max value
		CombineAllChildInputs(result, [&](InputState<glm::vec2>& inout_state, InputState<glm::vec2> const& child_state)
		{
			glm::vec2 const& child_previous_value = (child_state.update_time == frame_time)?
				child_state.previous_value :
				child_state.value;

			for (size_t axis : {0, 1})
			{
				UpdateMinAndMaxAxisValue(child_state.value[axis], min_neg_value[axis], max_pos_value[axis]);
				UpdateMinAndMaxAxisValue(child_previous_value[axis], previous_min_neg_value[axis], previous_max_pos_value[axis]);
			}
		});

		if (result.state.has_value())
		{
			for (size_t axis : {0, 1})
			{
				result.state->value[axis] = MergeAxisValues(min_neg_value[axis], max_pos_value[axis]);
				result.state->previous_value[axis] = MergeAxisValues(previous_min_neg_value[axis], previous_max_pos_value[axis]);
			}
		}

		return result;
	}
}

#endif

}; // namespace chaos