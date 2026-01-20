namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GENERATE_IS_ANY_OF_CONCEPT(InputType, Key, Input1D, Input2D);
	CHAOS_GENERATE_IS_ANY_OF_CONCEPT(InputTypeExt, Key, Input1D, Input2D, MappedInput1D, MappedInput2D);

	CHAOS_GENERATE_CLASS_MAPPING_DECLARATION(InputValueType, InputTypeExt);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputValueType, Key, bool);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputValueType, Input1D, float);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputValueType, Input2D, glm::vec2);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputValueType, MappedInput1D, float);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputValueType, MappedInput2D, glm::vec2);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<InputType INPUT_TYPE>
	InputDeviceType GetDeviceForInput(INPUT_TYPE in_input)
	{
		if (IsKeyboardInput(in_input))
			return InputDeviceType::KEYBOARD;
		if (IsMouseInput(in_input))
			return InputDeviceType::MOUSE;
		if (IsGamepadInput(in_input))
			return InputDeviceType::GAMEPAD;
		return InputDeviceType::UNKNOWN;
	}

	template<InputType INPUT_TYPE>
	char const* GetInputName(INPUT_TYPE in_input)
	{
		if (char const* result = EnumToString(in_input))
			return result;
		return "Unknown";
	}

#endif

}; // namespace chaos