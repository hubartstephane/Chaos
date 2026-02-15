#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos 
{
	static EnumMetaData<InputStatusCheckType> const InputStatusCheckType_metadata =
	{
		{ InputStatusCheckType::None, "None" },
		{ InputStatusCheckType::Inactive, "Inactive" },
		{ InputStatusCheckType::InactiveRepeated, "InactiveRepeated" },
		{ InputStatusCheckType::JustDeactivated, "JustDeactivated" },
		{ InputStatusCheckType::Active, "Active" },
		{ InputStatusCheckType::JustActivated, "JustActivated" },
		{ InputStatusCheckType::ActiveRepeated, "ActiveRepeated" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(InputStatusCheckType, &InputStatusCheckType_metadata, CHAOS_API);

}; // namespace chaos
