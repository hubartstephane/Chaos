#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	enum class WrapMode : int
	{
		none = 0,
		clamp = 1,
		wrap = 2,
		check_ranges = 3
	};

	/** JSON loading method */
	bool LoadFromJSON(nlohmann::json const& json_entry, WrapMode& dst);
	/** JSON saving method */
	bool SaveIntoJSON(nlohmann::json& json_entry, WrapMode const& src);

}; // namespace chaos
