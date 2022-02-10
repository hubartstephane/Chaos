namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Direction;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible direction */
	enum class Direction : int
	{
		POSITIVE_X = (1 << 0),
		NEGATIVE_X = (1 << 1),
		POSITIVE_Y = (1 << 2),
		NEGATIVE_Y = (1 << 3),
		POSITIVE_Z = (1 << 4),
		NEGATIVE_Z = (1 << 5)
	};

	CHAOS_DECLARE_ENUM_METHOD(Direction);

#endif

}; // namespace chaos