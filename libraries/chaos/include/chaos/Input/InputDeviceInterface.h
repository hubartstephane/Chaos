namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputDeviceInterface;

	enum class InputDeviceTypeMask;

	template<AnyInputType ANY_INPUT_TYPE>
	class InputStateQueryResult;

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
	* InputStateQueryResult: result when querying for an input
	*/

	template<AnyInputType ANY_INPUT_TYPE>
	class InputStateQueryResult : public std::optional<InputTypeToStateType_t<ANY_INPUT_TYPE>>
	{

	public:

		using state_type = InputTypeToStateType_t<ANY_INPUT_TYPE>;

		/** default constructor */
		InputStateQueryResult() = default;
		/** copy constructor */
		InputStateQueryResult(InputStateQueryResult const &) = default;
		/** constructor with initializer */
		InputStateQueryResult(std::optional<state_type> const& in_input_state, InputDeviceTypeMask in_involved_devices = InputDeviceTypeMask::None):
			std::optional<state_type>(in_input_state),
			involved_devices(in_involved_devices){}

	public:

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
		InputStateQueryResult<Key> GetInputState(Key input) const;
		/** gets any input1D state */
		InputStateQueryResult<Input1D> GetInputState(Input1D input) const;
		/** gets any input2D state */
		InputStateQueryResult<Input2D> GetInputState(Input2D input) const;
		/** gets any Mappedinput1D state */
		InputStateQueryResult<Input1D> GetInputState(MappedInput1D const & input) const;
		/** gets any Mappedinput2D state */
		InputStateQueryResult<Input2D> GetInputState(MappedInput2D const & input) const;
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
			if (auto input_state = GetInputState(in_input))
				return input_state->GetValue();
			return {};
		}

		/** get state status for any input */
		template<AnyInputType ANY_INPUT_TYPE>
		InputStatus GetInputStatus(ANY_INPUT_TYPE in_input) const
		{
			if (auto input_state = GetInputState(in_input))
				return input_state->GetStatus();
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
		InputStateQueryResult<BASIC_INPUT_TYPE> GetInputStateHelper(BASIC_INPUT_TYPE in_input) const;

		/** gets one key state */
		virtual InputStateQueryResult<Key> DoGetInputState(Key input) const;
		/** gets one input1D state */
		virtual InputStateQueryResult<Input1D> DoGetInputState(Input1D input) const;
		/** gets one input2D state */
		virtual InputStateQueryResult<Input2D> DoGetInputState(Input2D input) const;

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
	using input_value_type = CombinedInput<PARAMS...>::input_value_type;

	double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();

	// for all child_input, check find the state, prepare optional parameter if necessary, and call user function
	auto CombineAllChildInputs = [&](auto & inout_result, auto const & combine_child_input_func)
	{
		auto CombineInputStates = [&](auto & inout_result, auto const & in_child_state)
		{
			if (!in_child_state.has_value())
				return;
			if (!inout_result.has_value())
			{
				inout_result.emplace();
				inout_result->update_time = frame_time;
				if (in_child_state->IsActive())
					inout_result.involved_devices |= in_child_state.involved_devices;
			}
			combine_child_input_func(inout_result, in_child_state);
		};

		std::apply([&](auto const & ... child_input)
		{
			(CombineInputStates(inout_result, GetInputState(child_input)), ...);

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

	// KEY
	if constexpr (std::is_same_v<input_value_type, bool>)
	{
		InputStateQueryResult<Key> result;

		auto CombineChildInputFunc = [&](InputStateQueryResult<Key> & inout_result, InputStateQueryResult<Key> const & child_state)
		{
			bool child_previous_value = (child_state->update_time == frame_time) ?
				child_state->previous_value :
				child_state->value;

			inout_result->value |= child_state->value;
			inout_result->previous_value |= child_previous_value; // final combined input result is TRUE if at least one child input is TRUE
		};

		CombineAllChildInputs(result, CombineChildInputFunc);

		return result;
	}
	// INPUT1D
	else if constexpr (std::is_same_v<input_value_type, float>)
	{
		InputStateQueryResult<Input1D> result;

		float min_neg_value = 0.0f;
		float max_pos_value = 0.0f;
		float previous_min_neg_value = 0.0f;
		float previous_max_pos_value = 0.0f;

		// compute min & max value
		auto CombineChildInputFunc = [&](InputStateQueryResult<Input1D> & inout_result, InputStateQueryResult<Input1D> const & child_state)
		{
			float child_previous_value = (child_state->update_time == frame_time) ?
				child_state->previous_value :
				child_state->value;

			UpdateMinAndMaxAxisValue(child_state->value, min_neg_value, max_pos_value);
			UpdateMinAndMaxAxisValue(child_previous_value, previous_min_neg_value, previous_max_pos_value);
		};

		CombineAllChildInputs(result, CombineChildInputFunc);

		if (result.has_value())
		{
			result->value = MergeAxisValues(min_neg_value, max_pos_value);
			result->previous_value = MergeAxisValues(previous_min_neg_value, previous_max_pos_value);
		}

		return result;
	}
	// INPUT2D
	else if constexpr (std::is_same_v<input_value_type, glm::vec2>)
	{
		InputStateQueryResult<Input2D> result;

		glm::vec2 min_neg_value = { 0.0f, 0.0f };
		glm::vec2 max_pos_value = { 0.0f, 0.0f };
		glm::vec2 previous_min_neg_value = { 0.0f, 0.0f };
		glm::vec2 previous_max_pos_value = { 0.0f, 0.0f };

		// compute min & max value
		auto CombineChildInputFunc = [&](InputStateQueryResult<Input2D>& inout_result, InputStateQueryResult<Input2D> const& child_state)
		{
			glm::vec2 const & child_previous_value = (child_state->update_time == frame_time)?
				child_state->previous_value :
				child_state->value;

			for (size_t axis : {0, 1})
			{
				UpdateMinAndMaxAxisValue(child_state->value[axis], min_neg_value[axis], max_pos_value[axis]);
				UpdateMinAndMaxAxisValue(child_previous_value[axis], previous_min_neg_value[axis], previous_max_pos_value[axis]);
			}
		};

		CombineAllChildInputs(result, CombineChildInputFunc);

		if (result.has_value())
		{
			for (size_t axis : {0, 1})
			{
				result->value[axis] = MergeAxisValues(min_neg_value[axis], max_pos_value[axis]);
				result->previous_value[axis] = MergeAxisValues(previous_min_neg_value[axis], previous_max_pos_value[axis]);
			}
		}

		return result;
	}
}

#endif

}; // namespace chaos