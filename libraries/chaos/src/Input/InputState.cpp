#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos 
{
	static EnumMetaData<InputStateCheckType> const InputStateCheckType_metadata =
	{
		{ InputStateCheckType::None, "None" },
		{ InputStateCheckType::Inactive, "Inactive" },
		{ InputStateCheckType::InactiveRepeated, "InactiveRepeated" },
		{ InputStateCheckType::JustDeactivated, "JustDeactivated" },
		{ InputStateCheckType::Active, "Active" },
		{ InputStateCheckType::JustActivated, "JustActivated" },
		{ InputStateCheckType::ActiveRepeated, "ActiveRepeated" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(InputStateCheckType, &InputStateCheckType_metadata, CHAOS_API);

}; // namespace chaos
