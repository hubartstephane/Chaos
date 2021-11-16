namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Axis;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible axis */
	enum class Axis : int
	{
		AXIS_X = 0,
		AXIS_Y = 1,
		AXIS_Z = 2
	};

	CHAOS_DECLARE_ENUMJSON_METHOD(Axis);

#endif

}; // namespace chaos