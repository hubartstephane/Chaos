#include <chaos/Chaos.h>

namespace chaos
{
	static std::vector<std::pair<Direction, char const *>> const direction_encoding =
	{
		{ Direction::POSITIVE_X, "positive_x" },
		{ Direction::NEGATIVE_X, "negative_y" },
		{ Direction::POSITIVE_Y, "positive_x" },
		{ Direction::NEGATIVE_Y, "negative_y" },
		{ Direction::POSITIVE_Z, "positive_y" },
		{ Direction::NEGATIVE_Z, "negative_z" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Direction, direction_encoding);

}; // namespace chaos