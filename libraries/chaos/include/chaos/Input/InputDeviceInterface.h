namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputDeviceInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Some aliases
	*/

	using ForAllKeysFunction = LightweightFunction<bool(Key, KeyState const &)>;

	using ForAllInput1DFunction = LightweightFunction<bool(Input1D, Input1DState const &)>;

	using ForAllInput2DFunction = LightweightFunction<bool(Input2D, Input2DState const &)>;

	using EnumerateDeviceHierarchyFunction = LightweightFunction<bool(InputDeviceInterface const*)>;

	/**
	* InputDeviceInterface: a class to make request for some device state
	*/

	class CHAOS_API InputDeviceInterface
	{

	public:

		/** call a function on all devices handled by this whole hierarchy (composition pattern) */
		virtual bool EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const;

		/** gets any key state */
		std::optional<KeyState> GetInputState(Key input) const;
		/** gets any input1D state */
		std::optional<Input1DState> GetInputState(Input1D input) const;
		/** gets any input2D state */
		std::optional<Input2DState> GetInputState(Input2D input) const;
		/** gets any Mappedinput1D state */
		std::optional<Input1DState> GetInputState(MappedInput1D const & input) const;
		/** gets any Mappedinput2D state */
		std::optional<Input2DState> GetInputState(MappedInput2D const & input) const;
		/** gets any CompositeInput state */
		template<typename... PARAMS>
		requires SameInputType<PARAMS...>
		auto GetInputState(CombinedInput<PARAMS...> const & input) const;

		/** enumerate keys */
		bool ForAllKeys(ForAllKeysFunction func) const;
		/** enumerate input1D */
		bool ForAllInput1D(ForAllInput1DFunction func) const;
		/** enumerate input2D */
		bool ForAllInput2D(ForAllInput2DFunction func) const;

		/** get state value for any input */
		template<InputTypeExt INPUT_TYPE_EXT>
		InputValue_t<INPUT_TYPE_EXT> GetInputValue(INPUT_TYPE_EXT in_input) const
		{
			if (auto input_state = GetInputState(in_input))
				return input_state->GetValue();
			return {};
		}

		/** get state status for any input */
		template<InputTypeExt INPUT_TYPE_EXT>
		InputStatus GetInputStatus(INPUT_TYPE_EXT in_input) const
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
		template<InputType INPUT_TYPE>
		std::optional<InputState_t<INPUT_TYPE>> GetInputStateHelper(INPUT_TYPE in_input) const;

		/** gets one key state */
		virtual std::optional<KeyState> DoGetInputState(Key input) const;
		/** gets one input1D state */
		virtual std::optional<Input1DState> DoGetInputState(Input1D input) const;
		/** gets one input2D state */
		virtual std::optional<Input2DState> DoGetInputState(Input2D input) const;

		/** enumerate keys */
		virtual bool DoForAllKeys(ForAllKeysFunction func) const;
		/** enumerate input1D */
		virtual bool DoForAllInput1D(ForAllInput1DFunction func) const;
		/** enumerate input2D */
		virtual bool DoForAllInput2D(ForAllInput2DFunction func) const;
	};

#else

template<typename... PARAMS>
requires SameInputType<PARAMS...>
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
			}
			combine_child_input_func(inout_result, in_child_state);
		};

		std::apply([&](auto const & ... child_input)
		{
			(CombineInputStates(inout_result, GetInputState(child_input)), ...);

		}, input.child_inputs);
	};

	// KEY
	if constexpr (std::is_same_v<input_value_type, bool>)
	{
		std::optional<KeyState> result;

		auto CombineChildInputFunc = [&](std::optional<KeyState> & inout_result, std::optional<KeyState> const & child_state)
		{
			inout_result->value |= child_state->value;

			bool child_previous_value = (child_state->update_time == frame_time)?
				child_state->previous_value:
				child_state->value;
			inout_result->previous_value |= child_previous_value; // final combined input result is TRUE if at least one child input is TRUE
		};

		CombineAllChildInputs(result, CombineChildInputFunc);

		return result;
	}
	// INPUT1D
	else if constexpr (std::is_same_v<input_value_type, float>)
	{
		std::optional<Input1DState> result;

		float min_value = 0.0f;
		float max_value = 0.0f;
		float min_previous_value = 0.0f;
		float max_previous_value = 0.0f;

		// compute min & max value
		auto CombineChildInputFunc = [&](std::optional<Input1DState> & inout_result, std::optional<Input1DState> const & child_state)
		{
			if (child_state->value > 0.0f)
				max_value = std::max(max_value, child_state->value);
			else if (child_state->value < 0.0f)
				min_value = std::min(min_value, child_state->value);

			float child_previous_value = (child_state->update_time == frame_time) ?
				child_state->previous_value :
				child_state->value;

			if (child_previous_value > 0.0f)
				max_previous_value = std::max(max_value, child_previous_value);
			else if (child_previous_value < 0.0f)
				min_previous_value = std::min(min_value, child_previous_value);
		};

		CombineAllChildInputs(result, CombineChildInputFunc);

		if (result.has_value())
		{
			// if at least one child input is POS and one is NEG, let the value be 0
			if (max_value > 0.0f && min_value == 0.0f)
				result->value = max_value;
			else if (max_value == 0.0f && min_value < 0.0f)
				result->value = min_value;
			// if at least one child input is POS and one is NEG, let the previous value be 0
			if (max_previous_value > 0.0f && min_previous_value == 0.0f)
				result->previous_value = max_previous_value;
			else if (max_previous_value == 0.0f && min_previous_value < 0.0f)
				result->previous_value = min_previous_value;
		}

		return result;
	}
	// INPUT2D
	else if constexpr (std::is_same_v<input_value_type, glm::vec2>)
	{
		std::apply([&](auto const & ... child_input)
			{
				int i = 0;
				++i;

			}, input.child_inputs);
	}
}

#endif

}; // namespace chaos