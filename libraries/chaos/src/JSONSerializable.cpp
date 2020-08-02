#pragma once

#include <chaos/JSONSerializable.h>

namespace chaos
{

	bool JSONSerializable::SerializeIntoJSON(nlohmann::json& json_entry) const
	{
		return true;
	}

	bool JSONSerializable::SerializeFromJSON(nlohmann::json const& json_entry)
	{
		return true;
	}

}; // namespace chaos
