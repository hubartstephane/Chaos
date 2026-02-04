#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<Axis> const Axis_metadata =
	{
		{ Axis::AxisX, "axis_x" },
		{ Axis::AxisY, "axis_y" },
		{ Axis::AxisZ, "axis_z" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Axis, &Axis_metadata, CHAOS_API);

	glm::vec3 AxisToVector(Axis in_axis)
	{
		switch (in_axis)
		{
		case Axis::AxisX:
			return { 1.0f, 0.0f, 0.0f };
		case Axis::AxisY:
			return { 0.0f, 1.0f, 0.0f };
		case Axis::AxisZ:
			return { 0.0f, 0.0f, 1.0f };
		default:
			assert(0);
			return { 0.0f, 0.0f, 0.0f };
		}
	}

}; // namespace chaos