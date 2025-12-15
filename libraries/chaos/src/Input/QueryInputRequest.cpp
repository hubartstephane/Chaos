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

	QueryKeyInputRequest QueryInput(Key in_input, QueryInputRequestType in_query_type)
	{
		return { in_input, nullptr, nullptr, in_query_type };
	}

	QueryInput1DRequest QueryInput(Input1D in_input, QueryInputRequestType in_query_type)
	{
		return { in_input, nullptr, nullptr, in_query_type };
	}

	QueryInput2DRequest QueryInput(Input2D in_input, QueryInputRequestType in_query_type)
	{
		return { in_input, nullptr, nullptr, in_query_type };
	}

	QueryKeyInputRequest QueryInput(Key in_input, bool* out_value, QueryInputRequestType in_query_type)
	{
		return { in_input, nullptr, out_value, in_query_type };
	}

	QueryInput1DRequest QueryInput(Input1D in_input, float* out_value, QueryInputRequestType in_query_type)
	{
		return { in_input, nullptr, out_value, in_query_type };
	}

	QueryInput2DRequest QueryInput(Input2D in_input, glm::vec2* out_value, QueryInputRequestType in_query_type)
	{
		return { in_input, nullptr, out_value, in_query_type };
	}

	QueryKeyInputRequest QueryInput(Key in_input, KeyState* out_state, QueryInputRequestType in_query_type)
	{
		return { in_input, out_state, nullptr, in_query_type };
	}

	QueryInput1DRequest QueryInput(Input1D in_input, Input1DState* out_state, QueryInputRequestType in_query_type)
	{
		return { in_input, out_state, nullptr, in_query_type };
	}

	QueryInput2DRequest QueryInput(Input2D in_input, Input2DState* out_state, QueryInputRequestType in_query_type)
	{
		return { in_input, out_state, nullptr, in_query_type };
	}

}; // namespace chaos
