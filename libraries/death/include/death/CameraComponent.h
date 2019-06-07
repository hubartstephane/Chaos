#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GeometryFramework.h>
#include <chaos/Tickable.h>
#include <death/GameFramework.h>

namespace death
{

	// =============================================
	// CameraComponent
	// =============================================

	class CameraComponent : public chaos::Tickable
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** constructor */
		CameraComponent();

	protected:

		/** called whenever the component is inserted into the camera */
		virtual void OnInsertedInto(Camera * in_camera);
		/** called whenever the component is removed from the camera */
		virtual void OnRemovedFrom(Camera * in_camera);

	protected:

		/** the owning camera */
		Camera * camera = nullptr;
	};

	// =============================================
	// FollowPlayerCameraComponent
	// =============================================

	class FollowPlayerCameraComponent : public CameraComponent
	{
	public:

		/** constructor */
		FollowPlayerCameraComponent(int in_player_index = 0) :
			player_index(in_player_index) {}

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;

	protected:

		/** the player index to follow */
		int player_index = 0;
	};

}; // namespace death
