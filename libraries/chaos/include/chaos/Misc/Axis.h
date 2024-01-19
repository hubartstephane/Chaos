namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Axis;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible axis */
	enum class CHAOS_API Axis : int
	{
		AXIS_X = 0,
		AXIS_Y = 1,
		AXIS_Z = 2
	};

	CHAOS_DECLARE_ENUM_METHOD(Axis, CHAOS_API);

#endif

}; // namespace chaos