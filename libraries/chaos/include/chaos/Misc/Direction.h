namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Direction;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible direction */
	enum class Direction : int // order is important
	{
		NegativeX = 0,
		PositiveX = 1,
		NegativeY = 2,
		PositiveY = 3,
		NegativeZ = 4,
		PositiveZ = 5		
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