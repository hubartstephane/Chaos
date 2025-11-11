#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<Direction> const Direction_metadata =
	{
		{ Direction::POSITIVE_X, "positive_x" },
		{ Direction::NEGATIVE_X, "negative_y" },
		{ Direction::POSITIVE_Y, "positive_x" },
		{ Direction::NEGATIVE_Y, "negative_y" },
		{ Direction::POSITIVE_Z, "positive_y" },
		{ Direction::NEGATIVE_Z, "negative_z" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Direction, &Direction_metadata, CHAOS_API);

	glm::vec3 DirectionToVector(Direction in_direction)
	{
		switch (in_direction)
		{
		case Direction::NEGATIVE_X:
			return { -1.0f, 0.0f, 0.0f };
		case Direction::POSITIVE_X:
			return { 1.0f, 0.0f, 0.0f };
		case Direction::NEGATIVE_Y:
			return { 0.0f, -1.0f, 0.0f };
		case Direction::POSITIVE_Y:
			return { 0.0f, 1.0f, 0.0f };
		case Direction::NEGATIVE_Z:
			return { 0.0f, 0.0f, -1.0f };
		case Direction::POSITIVE_Z:
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
		case Direction::NEGATIVE_X:
		case Direction::POSITIVE_X:
			return Axis::AXIS_X;
		case Direction::NEGATIVE_Y:
		case Direction::POSITIVE_Y:
			return Axis::AXIS_Y;
		case Direction::NEGATIVE_Z:
		case Direction::POSITIVE_Z:
			return Axis::AXIS_Z;
		default:
			assert(0);
			return Axis::AXIS_X;
		}
	}

	float GetDirectionSign(Direction in_direction)
	{
		switch (in_direction)
		{
		case Direction::NEGATIVE_X:
		case Direction::NEGATIVE_Y:
		case Direction::NEGATIVE_Z:
			return -1.0f;
		case Direction::POSITIVE_X:
		case Direction::POSITIVE_Y:
		case Direction::POSITIVE_Z:
			return 1.0f;
		default:
			assert(0);
			return 0.0f;
		}
	}

}; // namespace chaos