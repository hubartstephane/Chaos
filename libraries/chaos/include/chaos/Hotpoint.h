#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	enum HotpointType : int // XXX : no class so it can be casted to int
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

	/** this class is used to handle hotpoint and there conversions */
	class Hotpoint
	{
	public:

		/** given a hotpoint and a hotpoint_type, returns the BottomLeft hotpoint position */
		static glm::vec2 ConvertToBottomLeft(glm::vec2 const & hotpoint, glm::vec2 const & size, HotpointType hotpoint_type);
		/** given a hotpoint and a hotpoint_type, returns any other hotpoint_type position */
		static glm::vec2 Convert(glm::vec2 const & hotpoint, glm::vec2 const & size, HotpointType initial_hotpoint_type, HotpointType final_hotpoint_type);

		/** data for JSON encoding/decoding */
		static std::vector<std::pair<HotpointType, char const *>> const hotpoint_encoding;
	};



}; // namespace chaos