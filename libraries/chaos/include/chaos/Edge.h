#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	/** the possible edges (XXX: values are related to TiledMap::Wang. do not change them without a check) */
	enum class Edge : int
	{
		TOP = 0,
		RIGHT = 1,
		BOTTOM = 2,
		LEFT = 3
	};

}; // namespace chaos
