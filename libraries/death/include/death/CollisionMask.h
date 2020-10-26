#pragma once

#include <Chaos/Chaos.h>

namespace death
{
	namespace CollisionMask
	{
		/** for object/layer ... that can collides with player */
		static constexpr int PLAYER = (1 << 0);
		/** for object/layer ... that can collides with camera */
		static constexpr int CAMERA = (1 << 1);
	};
	
}; // namespace death
