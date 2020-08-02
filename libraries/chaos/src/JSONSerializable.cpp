#pragma once

#include <chaos/JSONSerializable.h>

namespace chaos
{

	bool JSONSerializable::SerializeIntoJSON(nlohmann::json& json_entry) const
	{
		assert(json_entry.is_object());
		return true;
	}

	bool JSONSerializable::SerializeFromJSON(nlohmann::json const& json_entry)
	{
		assert(json_entry.is_object());
		return true;
	}

}; // namespace chaos
