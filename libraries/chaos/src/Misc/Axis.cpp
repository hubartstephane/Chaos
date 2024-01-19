#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static chaos::EnumTools::EnumMetaData<Axis> const Axis_metadata =
	{
		{ Axis::AXIS_X, "axis_x" },
		{ Axis::AXIS_Y, "axis_y" },
		{ Axis::AXIS_Z, "axis_z" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Axis, Axis_metadata, CHAOS_API);

}; // namespace chaos