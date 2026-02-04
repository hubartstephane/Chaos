#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<Direction> const Direction_metadata =
	{
		{ Direction::NegativeX, "negative_y" },
		{ Direction::PositiveX, "positive_x" },
		{ Direction::NegativeY, "negative_y" },
		{ Direction::PositiveY, "positive_x" },
		{ Direction::NegativeZ, "negative_z" },
		{ Direction::PositiveZ, "positive_y" },
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Direction, &Direction_metadata, CHAOS_API);

	glm::vec3 DirectionToVector(Direction in_direction)
	{
		switch (in_direction)
		{
		case Direction::NegativeX:
			return { -1.0f, 0.0f, 0.0f };
		case Direction::PositiveX:
			return { 1.0f, 0.0f, 0.0f };
		case Direction::NegativeY:
			return { 0.0f, -1.0f, 0.0f };
		case Direction::PositiveY:
			return { 0.0f, 1.0f, 0.0f };
		case Direction::NegativeZ:
			return { 0.0f, 0.0f, -1.0f };
		case Direction::PositiveZ:
			return { 0.0f, 0.0f, 1.0f };
		default:
			assert(0);
			return { 0.0f, 0.0f, 0.0f };
		}
	}

	Axis DirectionToAxis(Direction in_direction)
	{
		switch (in_direction)
		{
		case Direction::NegativeX:
		case Direction::PositiveX:
			return Axis::AxisX;
		case Direction::NegativeY:
		case Direction::PositiveY:
			return Axis::AxisY;
		case Direction::NegativeZ:
		case Direction::PositiveZ:
			return Axis::AxisZ;
		default:
			assert(0);
			return Axis::AxisX;
		}
	}

	float GetDirectionSign(Direction in_direction)
	{
		switch (in_direction)
		{
		case Direction::NegativeX:
		case Direction::NegativeY:
		case Direction::NegativeZ:
			return -1.0f;
		case Direction::PositiveX:
		case Direction::PositiveY:
		case Direction::PositiveZ:
			return 1.0f;
		default:
			assert(0);
			return 0.0f;
		}
	}

}; // namespace chaos