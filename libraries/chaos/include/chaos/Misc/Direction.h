namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Direction;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible direction */
	enum class CHAOS_API Direction : int
	{
		POSITIVE_X = (1 << 0),
		NEGATIVE_X = (1 << 1),
		POSITIVE_Y = (1 << 2),
		NEGATIVE_Y = (1 << 3),
		POSITIVE_Z = (1 << 4),
		NEGATIVE_Z = (1 << 5)
	};

	CHAOS_DECLARE_ENUM_METHOD(Direction, CHAOS_API);

	/** get the 3d vector corresponding to a direction */
	CHAOS_API glm::vec3 DirectionToVector(Direction in_direction);
	/** get the axis corresponding to a direction */
	CHAOS_API Axis DirectionToAxis(Direction in_direction);
	/** returns whether the direction is positive or negative */
	CHAOS_API float GetDirectionSign(Direction in_direction);

#endif

}; // namespace chaos