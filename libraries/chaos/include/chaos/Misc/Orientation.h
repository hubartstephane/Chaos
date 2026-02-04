namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Orientation;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible axis */
	enum class Orientation : int
	{
		Horizontal = 0,
		Vertical = 1
	};

	CHAOS_DECLARE_ENUM_METHOD(Orientation, CHAOS_API);

#endif

}; // namespace chaos