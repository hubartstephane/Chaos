#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/Tickable.h>

#include <death/CameraComponent.h>
#include <death/GameFramework.h>
#include <death/ComponentOwner.h>

namespace death
{

	// =============================================
	// CameraTransform
	// =============================================

	class CameraTransform
	{
	public:

		/** returns the transformation matrix from a camera obox */	
		static glm::mat4x4 GetCameraTransform(chaos::obox2 const & obox);
	};

	// =============================================
	// Camera
	// =============================================

	class Camera : public chaos::Tickable
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** constructor */
		Camera(LevelInstance * in_level_instance);

		/** get the player by its index */
		Player * GetPlayer(size_t player_index);
		/** get the player by its index */
		Player const * GetPlayer(size_t player_index) const;

		/** returns the game */
		Game * GetGame();
		/** returns the game */
		Game const * GetGame() const;

		/** returns the game instance */
		GameInstance * GetGameInstance();
		/** returns the game instance */
		GameInstance const * GetGameInstance() const;

		/** returns the level */
		Level * GetLevel();
		/** returns the level */
		Level const * GetLevel() const;

		/** returns the level instance */
		LevelInstance * GetLevelInstance() { return level_instance; }
		/** returns the level */
		LevelInstance const * GetLevelInstance() const { return level_instance; }

		/** set the camera box */
		void SetCameraBox(chaos::box2 const & in_box) { camera_box = in_box;}

		/** get the camera box */
		chaos::box2  GetCameraBox(bool apply_modifiers = true) const;
		/** get the camera OBox */
		chaos::obox2 GetCameraOBox(bool apply_modifiers = true) const;


		/** get the camera OBox when level instance is started */
		chaos::obox2 const & GetInitialCameraOBox() const { return initial_camera_obox;}
		/** set the camera initial OBox */
		void SetInitialCameraOBox(chaos::obox2 const & in_obox) { initial_camera_obox = in_obox; }

		/** get the safe zone of the camera */
		glm::vec2 const & GetSafeZone() const { return safe_zone; }
		/** set the safe zone of the camera */
		void SetSafeZone(glm::vec2 const & in_safe_zone) { safe_zone = in_safe_zone; }

		/** Camera is a CameraComponent owner */
		DEATH_DECLARE_COMPONENT_OWNER(CameraComponent, Component, components)

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** the camera bounding box */
		chaos::box2 camera_box;
		/** the initial camera obox (at level startup) */
		chaos::obox2 initial_camera_obox;
		/** the safe zone of the camera */
		glm::vec2 safe_zone = glm::vec2(0.8f, 0.8f);

		/** the game instance owning the camera */
		LevelInstance * level_instance = nullptr;

		/** the components */
		std::vector<chaos::shared_ptr<CameraComponent>> components;

	};

}; // namespace death
