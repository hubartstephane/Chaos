#include "chaos/ChaosPCH.h"
#pragma once

#include "chaos/ChaosInternals.h"

namespace chaos
{

	bool JSONSerializable::SerializeIntoJSON(nlohmann::json& json) const
	{
		assert(json.is_object());
		return true;
	}

	bool JSONSerializable::SerializeFromJSON(nlohmann::json const& json)
	{
		assert(json.is_object());
		return true;
	}

}; // namespace chaos
