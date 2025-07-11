#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static chaos::EnumTools::EnumMetaData<KeyActionMask> const KeyActionMask_metadata =
	{
		{ KeyActionMask::None, "None" },
		{ KeyActionMask::Release, "Release" },
		{ KeyActionMask::Press, "Press" },
		{ KeyActionMask::Repeat, "Repeat" },
		{ KeyActionMask::Down, "Down" },
		{ KeyActionMask::Any, "Any" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(KeyActionMask, &KeyActionMask_metadata, CHAOS_API);
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(KeyActionMask, nullptr, CHAOS_API);

}; // namespace chaos
