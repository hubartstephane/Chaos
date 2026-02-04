#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<WrapMode> const WrapMode_metadata =
	{
		{ WrapMode::None, "NONE" },
		{ WrapMode::Clamp, "CLAMP" },
		{ WrapMode::Wrap, "WRAP" },
		{ WrapMode::CheckLimits, "CHECK_RANGES" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(WrapMode, &WrapMode_metadata, CHAOS_API);

}; // namespace chaos