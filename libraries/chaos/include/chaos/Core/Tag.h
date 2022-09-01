namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

#define CHAOS_DECLARE_TAG(x) static chaos::TagType x = chaos::DeclareTag(#x)

	/** a tag is an integer that can reference any address */
	using TagType = uintptr_t;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** function to generate a TagType from a name (XXX : not thread safe) */
	CHAOS_API TagType DeclareTag(char const* name);

#endif

}; // namespace chaos