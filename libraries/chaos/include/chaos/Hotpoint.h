#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	/** this class is used to handle hotpoint and there conversions */
	class Hotpoint
	{
	public:

		/** given a hotpoint and a hotpoint_type, returns the BottomLeft hotpoint position */
		static glm::vec2 ConvertToBottomLeft(glm::vec2 const & hotpoint, glm::vec2 const & size, int hotpoint_type);
		/** given a hotpoint and a hotpoint_type, returns any other hotpoint_type position */
		static glm::vec2 Convert(glm::vec2 const & hotpoint, glm::vec2 const & size, int initial_hotpoint_type, int final_hotpoint_type);

		static int const LEFT = 1;
		static int const RIGHT = 2;
		static int const HMIDDLE = 0;

		static int const BOTTOM = 4;
		static int const TOP = 8;
		static int const VMIDDLE = 0;

		static int const BOTTOM_LEFT = BOTTOM | LEFT;
		static int const TOP_LEFT = TOP | LEFT;
		static int const BOTTOM_RIGHT = BOTTOM | RIGHT;
		static int const TOP_RIGHT = TOP | RIGHT;
		static int const CENTER = VMIDDLE | HMIDDLE;
	};
}; // namespace chaos