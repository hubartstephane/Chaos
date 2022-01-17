#include <chaos/Chaos.h>

namespace chaos
{
	static std::vector<std::pair<Orientation, char const *>> const orientation_encoding =
	{
		{ Orientation::HORIZONTAL, "horizontal" },
		{ Orientation::VERTICAL, "vertical" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Orientation, orientation_encoding);

}; // namespace chaos