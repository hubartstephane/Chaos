#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{

	Input1DMappingInfo const Input1DMappingInfo::default_keyboard_mapping =
	{
		Key::LEFT,
		Key::RIGHT
	};

	Input1DMappingInfo const Input1DMappingInfo::default_dpad_mapping =
	{
		Key::GAMEPAD_DPAD_LEFT,
		Key::GAMEPAD_DPAD_RIGHT
	};

	Input2DMappingInfo const Input2DMappingInfo::default_keyboard_mapping =
	{
		Key::LEFT,
		Key::RIGHT,
		Key::UP,
		Key::DOWN
	};

	Input2DMappingInfo const Input2DMappingInfo::default_dpad_mapping =
	{
		Key::GAMEPAD_DPAD_LEFT,
		Key::GAMEPAD_DPAD_RIGHT,
		Key::GAMEPAD_DPAD_UP,
		Key::GAMEPAD_DPAD_DOWN
	};

	QueryMapped1DInputRequest QueryInput(Input1DMappingInfo const& in_input, QueryInputRequestType in_query_type)
	{
		return QueryMapped1DInputRequest(in_input, nullptr, in_query_type);
	}

	QueryMapped2DInputRequest QueryInput(Input2DMappingInfo const& in_input, QueryInputRequestType in_query_type)
	{
		return QueryMapped2DInputRequest(in_input, nullptr, in_query_type);
	}

	QueryMapped1DInputRequest QueryInput(Input1DMappingInfo const& in_input, float* out_value, QueryInputRequestType in_query_type)
	{
		return QueryMapped1DInputRequest(in_input, out_value, in_query_type);
	}

	QueryMapped2DInputRequest QueryInput(Input2DMappingInfo const& in_input, glm::vec2* out_value, QueryInputRequestType in_query_type)
	{
		return QueryMapped2DInputRequest(in_input, out_value, in_query_type);
	}

}; // namespace chaos
