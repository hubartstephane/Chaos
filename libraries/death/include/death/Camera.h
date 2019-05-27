#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GeometryFramework.h>
#include <chaos/Tickable.h>
#include <death/GameFramework.h>

namespace death
{

	// =============================================
	// Camera
	// =============================================

	class Camera : public chaos::Tickable
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** constructor */
		Camera(GameLevelInstance * in_game_level_instance);

		/** get the camera box */
		chaos::box2 const & GetCameraBox() const { return camera_box;}
		/** get the camera box when level instance is started */
		chaos::box2 const & GetInitialCameraBox() const { return initial_camera_box;}
		/** get the safe zone of the camera */
		glm::vec2 const & GetSafeZone() const { return safe_zone; }

		/** set the camera box */
		void SetCameraBox(chaos::box2 const & in_box) { camera_box = in_box;}
		/** set the camera box */
		void SetInitialCameraBox(chaos::box2 const & in_box) { initial_camera_box = in_box; }
		/** set the safe zone of the camera */
		void SetSafeZone(glm::vec2 const & in_safe_zone) { safe_zone = in_safe_zone; }

	protected:

		/** the camera bounding box */
		chaos::box2 camera_box;
		/** the initial camera bounding box (at level startup) */
		chaos::box2 initial_camera_box;
		/** the safe zone of the camera */
		glm::vec2 safe_zone = glm::vec2(0.8f, 0.8f);

		/** the game instance owning the camera */
		GameLevelInstance * game_level_instance = nullptr;

	};

}; // namespace death
