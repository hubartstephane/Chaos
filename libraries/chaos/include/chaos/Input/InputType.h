namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GENERATE_IS_ANY_OF_CONCEPT(BasicInputType, Key, Input1D, Input2D);
	CHAOS_GENERATE_IS_ANY_OF_CONCEPT(MappedInputType, MappedInput1D, MappedInput2D);
	template<typename T>
	concept AnyInputType = BasicInputType<T> || MappedInputType<T>;

	CHAOS_GENERATE_CLASS_MAPPING_DECLARATION(InputTypeToValueType, AnyInputType);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputTypeToValueType, Key, bool);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputTypeToValueType, Input1D, float);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputTypeToValueType, Input2D, glm::vec2);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputTypeToValueType, MappedInput1D, float);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputTypeToValueType, MappedInput2D, glm::vec2);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<BasicInputType BASIC_INPUT_TYPE>
	InputDeviceType GetDeviceForInput(BASIC_INPUT_TYPE in_input)
	{
		if (IsKeyboardInput(in_input))
			return InputDeviceType::Keyboard;
		if (IsMouseInput(in_input))
			return InputDeviceType::Mouse;
		if (IsGamepadInput(in_input))
			return InputDeviceType::Gamepad;
		return InputDeviceType::Unknown;
	}

	template<BasicInputType BASIC_INPUT_TYPE>
	char const* GetInputName(BASIC_INPUT_TYPE in_input)
	{
		if (char const* result = EnumToString(in_input))
			return result;
		return "Unknown";
	}

#endif

}; // namespace chaos