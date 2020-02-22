#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	enum HotpointType : int // XXX : no class, so we can make implicit int conversion
	{
		LEFT = 1,
		RIGHT = 2,
		HMIDDLE = 0,

		BOTTOM = 4,
		TOP = 8,
		VMIDDLE = 0,

		BOTTOM_LEFT = BOTTOM | LEFT,
		TOP_LEFT = TOP | LEFT,
		BOTTOM_RIGHT = BOTTOM | RIGHT,
		TOP_RIGHT = TOP | RIGHT,
		CENTER = VMIDDLE | HMIDDLE
	};

	/** given a hotpoint and a hotpoint_type, returns the BottomLeft hotpoint position */
	glm::vec2 ConvertHotpointToBottomLeft(glm::vec2 const& hotpoint, glm::vec2 const& size, HotpointType hotpoint_type);
	/** given a hotpoint and a hotpoint_type, returns any other hotpoint_type position */
	glm::vec2 ConvertHotpoint(glm::vec2 const& hotpoint, glm::vec2 const& size, HotpointType initial_hotpoint_type, HotpointType final_hotpoint_type);

	/** JSON loading method */
	bool LoadFromJSON(nlohmann::json const& json_entry, HotpointType& dst);
	/** JSON saving method */
	bool SaveIntoJSON(nlohmann::json& json_entry, HotpointType const& src);

}; // namespace chaos