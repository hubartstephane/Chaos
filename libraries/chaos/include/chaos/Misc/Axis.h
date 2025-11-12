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

	/** get the 3d vector corresponding to an axis */
	CHAOS_API glm::vec3 AxisToVector(Axis in_axis);

#endif

}; // namespace chaos