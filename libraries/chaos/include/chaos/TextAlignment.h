#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	enum class TextAlignment : int
	{
		ALIGN_LEFT = 0,
		ALIGN_RIGHT = 1,
		ALIGN_CENTER = 2,
		ALIGN_JUSTIFY = 3
	};

	/** JSON loading method */
	bool LoadFromJSON(nlohmann::json const& json_entry, TextAlignment& dst);
	/** JSON saving method */
	bool SaveIntoJSON(nlohmann::json& json_entry, TextAlignment const& src);

}; // namespace chaos