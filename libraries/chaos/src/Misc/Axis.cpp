#include "chaos/Chaos.h"

namespace chaos
{
	static std::vector<std::pair<Axis, char const *>> const axis_encoding =
	{
		{ Axis::AXIS_X, "axis_x" },
		{ Axis::AXIS_Y, "axis_y" },
		{ Axis::AXIS_Z, "axis_z" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Axis, axis_encoding);

}; // namespace chaos