#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<InterpolationType> const InterpolationType_metadata =
	{
		{ InterpolationType::LINEAR, "LINEAR" },
		{ InterpolationType::EASE, "EASE" },
		{ InterpolationType::EASIER, "EASIER" },
		{ InterpolationType::POW_2, "POW_2" },
		{ InterpolationType::POW_3, "POW_3" },
		{ InterpolationType::POW_5, "POW_5" },
		{ InterpolationType::SIN, "SIN" },
		{ InterpolationType::EXP, "EXP" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(InterpolationType, &InterpolationType_metadata, CHAOS_API);

}; // namespace chaos
