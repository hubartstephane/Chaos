#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumTools::EnumMetaData<KeyModifier> const KeyModifier_metadata =
	{
		{ KeyModifier::None, "None" },
		{ KeyModifier::Shift, "Shift" },
		{ KeyModifier::Control, "Control" },
		{ KeyModifier::Alt, "Alt" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(KeyModifier, &KeyModifier_metadata, CHAOS_API);
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(KeyModifier, nullptr, CHAOS_API);

}; // namespace chaos
