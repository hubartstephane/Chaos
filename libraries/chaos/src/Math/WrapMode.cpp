#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumTools::EnumMetaData<WrapMode> const WrapMode_metadata =
	{
		{ WrapMode::NONE, "NONE" },
		{ WrapMode::CLAMP, "CLAMP" },
		{ WrapMode::WRAP, "WRAP" },
		{ WrapMode::CHECK_RANGES, "CHECK_RANGES" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(WrapMode, &WrapMode_metadata, CHAOS_API);

}; // namespace chaos