#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	enum class TextAlignment : int
	{
		LEFT = 0,
		RIGHT = 1,
		CENTER = 2,
		JUSTIFY = 3
	};

	/** JSON loading method */
	bool LoadFromJSON(nlohmann::json const& json_entry, TextAlignment& dst);
	/** JSON saving method */
	bool SaveIntoJSON(nlohmann::json& json_entry, TextAlignment const& src);

}; // namespace chaos