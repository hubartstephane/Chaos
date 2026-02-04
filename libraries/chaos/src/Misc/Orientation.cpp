#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<Orientation> const Orientation_metadata =
	{
		{ Orientation::Horizontal, "horizontal" },
		{ Orientation::Vertical, "vertical" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Orientation, &Orientation_metadata, CHAOS_API);

}; // namespace chaos