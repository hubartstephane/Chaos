#pragma once

#include <chaos/StandardHeaders.h>
#include <death/CameraComponent.h>

namespace death
{

	// =============================================
	// FollowPlayerCameraComponent
	// =============================================

	class FollowPlayerCameraComponent : public CameraComponent
	{
	public:

		/** constructor */
		FollowPlayerCameraComponent(size_t in_player_index = 0) :
			player_index(in_player_index) {}

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** the player index to follow */
		size_t player_index = 0;
	};

}; // namespace death
