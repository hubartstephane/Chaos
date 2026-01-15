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

#define CHAOS_IMPLEMENT_QUERY_INPUT(INPUT_TYPE, QUERY_INPUT_REQUEST_PARAM_TYPE)\
	InspectInputCondition<QUERY_INPUT_REQUEST_PARAM_TYPE> QueryInput(INPUT_TYPE in_input, QueryInputRequestType in_query_type)\
	{\
		return InspectInputCondition<QUERY_INPUT_REQUEST_PARAM_TYPE>(in_input, nullptr, nullptr, in_query_type);\
	}\
	InspectInputCondition<QUERY_INPUT_REQUEST_PARAM_TYPE> QueryInput(INPUT_TYPE in_input, InputValueType_t<INPUT_TYPE> *out_value, QueryInputRequestType in_query_type)\
	{\
		return InspectInputCondition<QUERY_INPUT_REQUEST_PARAM_TYPE>(in_input, nullptr, out_value, in_query_type);\
	}\
	InspectInputCondition<QUERY_INPUT_REQUEST_PARAM_TYPE> QueryInput(INPUT_TYPE in_input, InputState_t<INPUT_TYPE>* out_state, QueryInputRequestType in_query_type)\
	{\
		return InspectInputCondition<QUERY_INPUT_REQUEST_PARAM_TYPE>(in_input, out_state, nullptr, in_query_type);\
	}

	CHAOS_IMPLEMENT_QUERY_INPUT(Key, Key);
	CHAOS_IMPLEMENT_QUERY_INPUT(Input1D, Input1D);
	CHAOS_IMPLEMENT_QUERY_INPUT(Input2D, Input2D);
	CHAOS_IMPLEMENT_QUERY_INPUT(MappedInput1D, MappedInput1D);
	CHAOS_IMPLEMENT_QUERY_INPUT(MappedInput2D, MappedInput2D);
	CHAOS_IMPLEMENT_QUERY_INPUT(TaggedInput<Key>, Key);
	CHAOS_IMPLEMENT_QUERY_INPUT(TaggedInput<Input1D>, Input1D);
	CHAOS_IMPLEMENT_QUERY_INPUT(TaggedInput<Input2D>, Input2D);
	CHAOS_IMPLEMENT_QUERY_INPUT(TaggedInput<MappedInput1D>, MappedInput1D);
	CHAOS_IMPLEMENT_QUERY_INPUT(TaggedInput<MappedInput2D>, MappedInput2D);

#undef CHAOS_IMPLEMENT_QUERY_INPUT

}; // namespace chaos
