#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumTools::EnumMetaData<Direction> const Direction_metadata =
	{
		{ Direction::POSITIVE_X, "positive_x" },
		{ Direction::NEGATIVE_X, "negative_y" },
		{ Direction::POSITIVE_Y, "positive_x" },
		{ Direction::NEGATIVE_Y, "negative_y" },
		{ Direction::POSITIVE_Z, "positive_y" },
		{ Direction::NEGATIVE_Z, "negative_z" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Direction, &Direction_metadata, CHAOS_API);

}; // namespace chaos