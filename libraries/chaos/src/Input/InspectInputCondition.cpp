#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<QueryInputRequestType> const QueryStatusRequestType_metadata =
	{
		{ QueryInputRequestType::None, "None" },
		{ QueryInputRequestType::Inactive, "Inactive" },
		{ QueryInputRequestType::InactiveRepeated, "InactiveRepeated" },
		{ QueryInputRequestType::JustDeactivated, "JustDeactivated" },
		{ QueryInputRequestType::Active, "Active" },
		{ QueryInputRequestType::JustActivated, "JustActivated" },
		{ QueryInputRequestType::ActiveRepeated, "ActiveRepeated" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(QueryInputRequestType, &QueryStatusRequestType_metadata, CHAOS_API);

#undef CHAOS_IMPLEMENT_QUERY_INPUT

}; // namespace chaos
