namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class TaggedInputFlags;

	template<InputTypeExt INPUT_TYPE_EXT>
	class TaggedInput;

	CHAOS_GENERATE_IS_ANY_OF_CONCEPT(TaggedInputType, TaggedInput<Key>, TaggedInput<Input1D>, TaggedInput<Input2D>);
	CHAOS_GENERATE_IS_ANY_OF_CONCEPT(TaggedInputTypeExt, TaggedInput<Key>, TaggedInput<Input1D>, TaggedInput<Input2D>, TaggedInput<MappedInput1D>, TaggedInput<MappedInput2D>);

	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputValueType, TaggedInput<Key>, bool);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputValueType, TaggedInput<Input1D>, float);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputValueType, TaggedInput<Input2D>, glm::vec2);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputValueType, TaggedInput<MappedInput1D>, float);
	CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(InputValueType, TaggedInput<MappedInput2D>, glm::vec2);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * TaggedInputFlags: a bitmask that defines which behaviors may be attached to a basic input
	 */

	enum class TaggedInputFlags : int
	{
		NONE = 0,
		CONSULT_ONLY = 1
	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(TaggedInputFlags, CHAOS_API);

	/**
	 * TaggedInput: a template class to give so extra behaviour to an input when resolving input management
	 */

	template<InputTypeExt INPUT_TYPE_EXT>
	class TaggedInput
	{
	public:

		using input_type = INPUT_TYPE_EXT;

		/** initializer constructor */
		TaggedInput(input_type in_input, TaggedInputFlags in_flags = TaggedInputFlags::NONE) :
			input(in_input),
			flags(in_flags){}
		/** copy constructor */
		TaggedInput(TaggedInput const& src) = default;

		/** cast operator */
		operator input_type () const { return input; }

	public:

		/** the input of interest */
		input_type input = input_type::UNKOWN;
		/** the tags */
		TaggedInputFlags flags = TaggedInputFlags::NONE;
	};

	/** get the input from a basic input (key, input1D, input2D) */
	template<InputTypeExt INPUT_TYPE_EXT>
	INPUT_TYPE_EXT GetInput(INPUT_TYPE_EXT in_input)
	{
		return in_input;
	}

	/** get the input from a tagged input */
	template<TaggedInputTypeExt TAGGED_INPUT_TYPE_EXT>
	auto GetInput(TAGGED_INPUT_TYPE_EXT in_input)
	{
		return in_input.input;
	}

	/** get the input flags from a basic input (key, input1D, input2D) */
	template<InputTypeExt INPUT_TYPE_EXT>
	auto GetInputFlags(INPUT_TYPE_EXT in_input)
	{
		return TaggedInputFlags::NONE;
	}

	/** get the input flags from a tagged input */
	template<TaggedInputTypeExt TAGGED_INPUT_TYPE_EXT>
	auto GetInputFlags(TAGGED_INPUT_TYPE_EXT in_input)
	{
		return in_input.flags;
	}

	/** add the flag CONSULT_ONLY to a basic input */
	template<InputTypeExt INPUT_TYPE_EXT>
	TaggedInput<INPUT_TYPE_EXT> ConsultOnly(INPUT_TYPE_EXT in_input) // the input won't lock any further request (it will still require for an authorization from the cache)
	{
		return { in_input , TaggedInputFlags::CONSULT_ONLY };
	}

	/** add the flag CONSULT_ONLY to a tagged input */
	template<TaggedInputTypeExt TAGGED_INPUT_TYPE_EXT>
	TAGGED_INPUT_TYPE_EXT ConsultOnly(TAGGED_INPUT_TYPE_EXT in_input) // the input won't lock any further request (it will still require for an authorization from the cache)
	{
		return { GetInput(in_input) , GetInputFlags(in_input) | TaggedInputFlags::CONSULT_ONLY };
	}

#endif

}; // namespace chaos