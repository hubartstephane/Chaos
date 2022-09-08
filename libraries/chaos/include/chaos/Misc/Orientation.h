namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Orientation;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible axis */
	enum class CHAOS_API Orientation : int
	{
		HORIZONTAL = 0,
		VERTICAL = 1
	};

	CHAOS_DECLARE_ENUM_METHOD(Orientation);

#endif

}; // namespace chaos