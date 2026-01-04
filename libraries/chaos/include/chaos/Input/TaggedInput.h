namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class TaggedInputFlags;

	template<InputTypeExt INPUT_TYPE_EXT>
	class TaggedInput;

	CHAOS_DECLARE_CLASS_VALUE_MAPPING(IsTaggedInput, bool, false);
	CHAOS_SPECIALIZE_CLASS_VALUE_MAPPING(IsTaggedInput, TaggedInput<Key>, bool, true);
	CHAOS_SPECIALIZE_CLASS_VALUE_MAPPING(IsTaggedInput, TaggedInput<Input1D>, bool, true);
	CHAOS_SPECIALIZE_CLASS_VALUE_MAPPING(IsTaggedInput, TaggedInput<Input2D>, bool, true);
	CHAOS_SPECIALIZE_CLASS_VALUE_MAPPING(IsTaggedInput, TaggedInput<MappedInput1D>, bool, true);
	CHAOS_SPECIALIZE_CLASS_VALUE_MAPPING(IsTaggedInput, TaggedInput<MappedInput2D>, bool, true);


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

		/** initializer constructor */
		TaggedInput(INPUT_TYPE_EXT in_input, TaggedInputFlags in_flags = TaggedInputFlags::NONE) :
			input(in_input),
			flags(in_flags){}
		/** copy constructor */
		TaggedInput(TaggedInput const& src) = default;

		/** cast operator */
		operator INPUT_TYPE_EXT () const { return input; }

	public:

		/** the input of interest */
		INPUT_TYPE_EXT input = INPUT_TYPE_EXT::UNKOWN;
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
	template<InputTypeExt INPUT_TYPE_EXT>
	INPUT_TYPE_EXT GetInput(TaggedInput<INPUT_TYPE_EXT> in_input)
	{
		return in_input.input;
	}

	/** get the input flags from a basic input (key, input1D, input2D) */
	template<InputTypeExt INPUT_TYPE_EXT>
	TaggedInputFlags GetInputFlags(INPUT_TYPE_EXT in_input)
	{
		return TaggedInputFlags::NONE;
	}

	/** get the input flags from a tagged input */
	template<InputTypeExt INPUT_TYPE_EXT>
	TaggedInputFlags GetInputFlags(TaggedInput<INPUT_TYPE_EXT> in_input)
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
	template<InputTypeExt INPUT_TYPE_EXT>
	TaggedInput<INPUT_TYPE_EXT> ConsultOnly(TaggedInput<INPUT_TYPE_EXT> in_input) // the input won't lock any further request (it will still require for an authorization from the cache)
	{
		return { GetInput(in_input) , GetInputFlags(in_input) | TaggedInputFlags::CONSULT_ONLY };
	}

#endif

}; // namespace chaos