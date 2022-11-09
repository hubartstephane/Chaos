#include "chaos/ChaosPCH.h"
#pragma once

#include "chaos/ChaosInternals.h"

namespace chaos
{

	bool JSONSerializableInterface::SerializeIntoJSON(nlohmann::json& json) const
	{
		assert(json.is_object());
		return true;
	}

	bool JSONSerializableInterface::SerializeFromJSON(nlohmann::json const& json)
	{
		assert(json.is_object());
		return true;
	}

}; // namespace chaos
