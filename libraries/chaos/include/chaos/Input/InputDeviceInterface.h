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
		std::optional<Input1DState> GetInputState(MappedInput1D input) const;
		/** gets any Mappedinput2D state */
		std::optional<Input2DState> GetInputState(MappedInput2D input) const;

		/** enumerate keys */
		bool ForAllKeys(ForAllKeysFunction func) const;
		/** enumerate input1D */
		bool ForAllInput1D(ForAllInput1DFunction func) const;
		/** enumerate input2D */
		bool ForAllInput2D(ForAllInput2DFunction func) const;

		/** get state value for any input */
		template<InputTypeExt INPUT_TYPE_EXT>
		InputValueType_t<INPUT_TYPE_EXT> GetInputValue(INPUT_TYPE_EXT in_input) const
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
			return InputStatus::NONE;
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

#endif

}; // namespace chaos