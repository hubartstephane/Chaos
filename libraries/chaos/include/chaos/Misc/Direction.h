#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	enum Direction;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

namespace chaos
{
	/** the possible direction */
	enum Direction : int // XXX: no class, so this can be implicitly converted to int
	{
		POSITIVE_X = (1 << 0),
		NEGATIVE_X = (1 << 1),
		POSITIVE_Y = (1 << 2),
		NEGATIVE_Y = (1 << 3),
		POSITIVE_Z = (1 << 4),
		NEGATIVE_Z = (1 << 5)
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION