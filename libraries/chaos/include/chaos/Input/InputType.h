namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	/** the data types a proper input may produce */
	CHAOS_GENERATE_IS_ANY_OF_CONCEPT(InputValueType, bool, float, glm::vec2);

	/** concept to check whether an input class declares the data it produces */
	template<typename T>
	concept HasInputValueType = requires
	{
		typename T::input_value_type;
		requires InputValueType<typename T::input_value_type>;
	};

	/** the different kind of inputs */
	CHAOS_GENERATE_IS_ANY_OF_CONCEPT(BasicInputType, Key, Input1D, Input2D);
	CHAOS_GENERATE_IS_ANY_OF_CONCEPT(MappedInputType, MappedInput1D, MappedInput2D);
	template<typename T>
	concept AnyInputType = BasicInputType<T> || MappedInputType<T> || HasInputValueType<T>;

	/** getting from an input class the kind of data it produces */
	CHAOS_GENERATE_CLASS_MAPPING_DECLARATION(InputTypeToValueType, AnyInputType);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputTypeToValueType, Key, bool);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputTypeToValueType, Input1D, float);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputTypeToValueType, Input2D, glm::vec2);
	
	// specialization for MappedInputXXX and CombinedInput
	template<AnyInputType T>
	requires HasInputValueType<T>
	struct InputTypeToValueType<T> : public boost::mpl::identity<
		typename T::input_value_type
	>{};
	
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