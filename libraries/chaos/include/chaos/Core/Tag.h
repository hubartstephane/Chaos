#ifdef CHAOS_FORWARD_DECLARATION

#define CHAOS_DECLARE_TAG(x) static chaos::TagType x = chaos::DeclareTag(#x)

namespace chaos
{
	/** a tag is an integer that can reference any address */
	using TagType = uintptr_t;

}; // namespace chaos

#else 

namespace chaos
{
	/** function to generate a TagType from a name (XXX : not thread safe) */
	TagType DeclareTag(char const * name);

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


