#pragma once

#include <chaos/JSONSerializable.h>

namespace chaos
{

	bool JSONSerializable::SaveIntoJSON(nlohmann::json& json_entry) const
	{
		return true;
	}

	bool JSONSerializable::LoadFromJSON(nlohmann::json const& json_entry)
	{
		return true;
	}

}; // namespace chaos
