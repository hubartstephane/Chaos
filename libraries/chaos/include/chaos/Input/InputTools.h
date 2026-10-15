namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class VirtualKeyDefinition;

	class VirtualInput2DDefinition;

	using VirtualKeyDefinitionEnumerationFunction = LightweightFunction<bool(VirtualKeyDefinition const &)>;

	using VirtualInput2DDefinitionEnumerationFunction = LightweightFunction<bool(VirtualInput2DDefinition const &)>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * VirtualKeyDefinition: describe a relation between a Key and an Input1D. In practise, the key is just an interpretation of the Input1D as a bool input
	 */

	class VirtualKeyDefinition
	{
	public:

		Key     key     = Key::Unknown;
		Input1D input1D = Input1D::Unknown;
	};

	/**
	 * VirtualInput2DDefinition: describe a relation between a Input2D and two Input1D. In practise, the Input2D is just an interpretation of the two Input1D as a vec2 input
	 */

	class VirtualInput2DDefinition
	{
	public:

		Input2D input2D   = Input2D::Unknown;
		Input1D input1D_x = Input1D::Unknown;
		Input1D input1D_y = Input1D::Unknown;
	};

	namespace InputTools
	{
		/** enumerate all relations Key <-> input1D */
		bool CHAOS_API EnumerateVirtualKeyDefinition(VirtualKeyDefinitionEnumerationFunction func);
		/** enumerate all relations input2D <-> Input1D, Input1D */
		bool CHAOS_API EnumerateVirtualInput2DDefinition(VirtualInput2DDefinitionEnumerationFunction func);

		/** enumerate all relations involving some Key */
		bool CHAOS_API EnumerateRelatedInputsDefinition(Key in_input, VirtualKeyDefinitionEnumerationFunction func);
		/** enumerate all relations involving some Input1D */
		bool CHAOS_API EnumerateRelatedInputsDefinition(Input1D in_input, VirtualKeyDefinitionEnumerationFunction key_func, VirtualInput2DDefinitionEnumerationFunction input2D_func);
		/** enumerate all relations involving some Input2D */
		bool CHAOS_API EnumerateRelatedInputsDefinition(Input2D in_input, VirtualInput2DDefinitionEnumerationFunction func);

		/** get the device corresponding to a basic input */
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

		/** get the name from a basic input */
		template<BasicInputType BASIC_INPUT_TYPE>
		char const* GetBasicInputName(BASIC_INPUT_TYPE in_input)
		{
			if (char const* result = EnumToString(in_input))
				return result;
			return "Unknown";
		}

		/** returns whether two basic inputs are related one to the other */
		template<BasicInputType BASIC_INPUT_TYPE1, BasicInputType BASIC_INPUT_TYPE2>
		bool IsBasicInputRelatedTo(BASIC_INPUT_TYPE1 in_input1, BASIC_INPUT_TYPE2 in_input2)
		{
			if constexpr (std::is_same_v<BASIC_INPUT_TYPE1, BASIC_INPUT_TYPE2>)
			{
				return (in_input1 == in_input2);
			}
			return false;
		}

		/** returns whether a basic input is related to one MappedInput1D */
		template<BasicInputType BASIC_INPUT_TYPE>
		bool IsBasicInputRelatedTo(BASIC_INPUT_TYPE in_input1, MappedInput1D const & in_input2)
		{
			if constexpr (std::is_same_v<BASIC_INPUT_TYPE, Key>)
			{
				return 
					IsBasicInputRelatedTo(in_input1, in_input2.neg_key) ||
					IsBasicInputRelatedTo(in_input1, in_input2.neg_key);
			}
			return false;
		}

		/** returns whether a basic input is related to one MappedInput2D */
		template<BasicInputType BASIC_INPUT_TYPE>
		bool IsBasicInputRelatedTo(BASIC_INPUT_TYPE in_input1, MappedInput2D const& in_input2)
		{
			if constexpr (std::is_same_v<BASIC_INPUT_TYPE, Key>)
			{
				return
					IsBasicInputRelatedTo(in_input1, in_input2.left_key)  ||
					IsBasicInputRelatedTo(in_input1, in_input2.right_key) ||
					IsBasicInputRelatedTo(in_input1, in_input2.up_key)    ||
					IsBasicInputRelatedTo(in_input1, in_input2.down_key);
			}
			return false;
		}

		/** returns whether a coimbined input is related to one MappedInput1D */
		template<BasicInputType BASIC_INPUT_TYPE, typename... PARAMS>
		requires HaveSameInputValueType<PARAMS...>
		bool IsBasicInputRelatedTo(BASIC_INPUT_TYPE in_input1, CombinedInput<PARAMS...> const& in_input2)
		{
			return std::apply([&](auto const & ... in_child_input)
			{
				return (IsBasicInputRelatedTo(in_input1, in_child_input) || ...);

			}, in_input2.child_inputs);
		}

		/** get the formated string for a basic input */
		template<BasicInputType BASIC_INPUT_TYPE>
		size_t GetInputString(BASIC_INPUT_TYPE in_input, char * in_buffer, size_t in_bufsize)
		{
			return StringTools::Strncpy(in_buffer, in_bufsize, GetBasicInputName(in_input));
		}

		/** get the formated string for a MappedInput1D */
		size_t CHAOS_API GetInputString(MappedInput1D const & in_input, char * in_buffer, size_t in_bufsize);

		/** get the formated string for a MappedInput2D */
		size_t CHAOS_API GetInputString(MappedInput2D const & in_input, char * in_buffer, size_t in_bufsize);

		/** get the formated string for a CombinedInput */
		template<typename... PARAMS>
		requires HaveSameInputValueType<PARAMS...>
		size_t GetInputString(CombinedInput<PARAMS...> const& in_input, char* in_buffer, size_t in_bufsize)
		{
			return std::apply([&](auto const & ... in_child_input)
			{
				size_t initial_bufsize = in_bufsize;

				auto AppendToBufferAndAdvance = [&](char const * src) mutable
				{
					return StringTools::AppendToBufferAndAdvance(in_buffer, in_bufsize, src);
				};

				size_t child_count = 0;

				auto AppendChildInputNameAndAdvance = [&](auto const & in_input) mutable
				{
					if (child_count > 0)
						if (!AppendToBufferAndAdvance(" / "))
							return false;

					size_t len = GetInputString(in_input, in_buffer, in_bufsize);
					in_buffer  += len;
					in_bufsize -= len;
					++child_count;
					return in_bufsize > 1;
				};

				AppendToBufferAndAdvance("Combined(") &&
				(AppendChildInputNameAndAdvance(in_child_input) && ...) && // stop has soon as there are no room in buffer anymore
				AppendToBufferAndAdvance(")");

				return size_t(initial_bufsize - in_bufsize);

			}, in_input.child_inputs);
		}














	}; // namespace InputTools

#endif

}; // namespace chaos