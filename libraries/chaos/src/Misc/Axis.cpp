#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<Axis> const Axis_metadata =
	{
		{ Axis::AXIS_X, "axis_x" },
		{ Axis::AXIS_Y, "axis_y" },
		{ Axis::AXIS_Z, "axis_z" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Axis, &Axis_metadata, CHAOS_API);

	glm::vec3 AxisToVector(Axis in_axis)
	{
		switch (in_axis)
		{
		case Axis::AXIS_X:
			return { 1.0f, 0.0f, 0.0f };
		case Axis::AXIS_Y:
			return { 0.0f, 1.0f, 0.0f };
		case Axis::AXIS_Z:
			return { 0.0f, 0.0f, 1.0f };
		default:
			assert(0);
			return { 0.0f, 0.0f, 0.0f };
		}
	}

}; // namespace chaos