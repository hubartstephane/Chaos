#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<InterpolationType> const InterpolationType_metadata =
	{
		{ InterpolationType::Linear, "LINEAR" },
		{ InterpolationType::Ease, "EASE" },
		{ InterpolationType::Easier, "EASIER" },
		{ InterpolationType::Pow2, "POW_2" },
		{ InterpolationType::Pow3, "POW_3" },
		{ InterpolationType::Pow5, "POW_5" },
		{ InterpolationType::Sin, "SIN" },
		{ InterpolationType::Exp, "EXP" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(InterpolationType, &InterpolationType_metadata, CHAOS_API);

}; // namespace chaos
