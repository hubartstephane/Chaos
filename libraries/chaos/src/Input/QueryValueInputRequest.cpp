#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<QueryStatusRequestType> const QueryStatusRequestType_metadata =
	{
		{ QueryStatusRequestType::None, "None" },
		{ QueryStatusRequestType::Inactive, "Inactive" },
		{ QueryStatusRequestType::InactiveRepeated, "InactiveRepeated" },
		{ QueryStatusRequestType::JustDeactivated, "JustDeactivated" },
		{ QueryStatusRequestType::Active, "Active" },
		{ QueryStatusRequestType::JustActivated, "JustActivated" },
		{ QueryStatusRequestType::ActiveRepeated, "ActiveRepeated" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(QueryStatusRequestType, &QueryStatusRequestType_metadata, CHAOS_API);


	QueryValueInputRequest<Key, KeyState, bool> QueryValue(Key in_key, QueryStatusRequestType in_query_type)
	{
		return { in_key, nullptr, nullptr, in_query_type };
	}

	QueryValueInputRequest<Input1D, Input1DState, float> QueryValue(Input1D in_axis, QueryStatusRequestType in_query_type)
	{
		return { in_axis, nullptr, nullptr, in_query_type };
	}

	QueryValueInputRequest<Input2D, Input2DState, glm::vec2> QueryValue(Input2D in_stick, QueryStatusRequestType in_query_type)
	{
		return { in_stick, nullptr, nullptr, in_query_type };
	}

	QueryValueInputRequest<Key, KeyState, bool> QueryValue(Key in_key, bool* out_value, QueryStatusRequestType in_query_type)
	{
		return { in_key, nullptr, out_value, in_query_type };
	}

	QueryValueInputRequest<Input1D, Input1DState, float> QueryValue(Input1D in_axis, float* out_value, QueryStatusRequestType in_query_type)
	{
		return { in_axis, nullptr, out_value, in_query_type };
	}

	QueryValueInputRequest<Input2D, Input2DState, glm::vec2> QueryValue(Input2D in_stick, glm::vec2* out_value, QueryStatusRequestType in_query_type)
	{
		return { in_stick, nullptr, out_value, in_query_type };
	}

	QueryValueInputRequest<Key, KeyState, bool> QueryValue(Key in_key, KeyState* out_state, QueryStatusRequestType in_query_type)
	{
		return { in_key, out_state, nullptr, in_query_type };
	}

	QueryValueInputRequest<Input1D, Input1DState, float> QueryValue(Input1D in_axis, Input1DState* out_state, QueryStatusRequestType in_query_type)
	{
		return { in_axis, out_state, nullptr, in_query_type };
	}

	QueryValueInputRequest<Input2D, Input2DState, glm::vec2> QueryValue(Input2D in_stick, Input2DState* out_state, QueryStatusRequestType in_query_type)
	{
		return { in_stick, out_state, nullptr, in_query_type };
	}

}; // namespace chaos
