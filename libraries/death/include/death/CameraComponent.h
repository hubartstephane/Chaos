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
		/** modifier */
		virtual chaos::box2 ApplyModifier(chaos::box2 const & src) const;

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
	// ShakeCameraComponent
	// =============================================

	class ShakeCameraComponent : public CameraComponent
	{
	public:

		/** constructor */
		ShakeCameraComponent(float in_modifier_duration, float in_modifier_range, float in_modifier_frequency):
			modifier_duration(in_modifier_duration),
			modifier_range(in_modifier_range),
			modifier_frequency(in_modifier_frequency){}

		/** override */
		virtual chaos::box2 ApplyModifier(chaos::box2 const & src) const override;

		/** the modifier is restarted */
		void RestartModifier();
		/** stop the modifier */
		void StopModifier();

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;

	protected:

		/** the duration of the shake effect (before the range fallbacks to 0) */
		float modifier_duration = 1.0f;
		/** the amplitude of the shake effet */
		float modifier_range = 1.0f;
		/** the frequency of the shake effect */
		float modifier_frequency = 0.1f;


		/** the time of the effect */
		float current_time = -1.0f;
		/** the current range of the effect */
		float current_range = 0.0f;
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

	// =============================================
	// FreeCameraComponent
	// =============================================

	class FreeCameraComponent : public CameraComponent
	{
	public:

		/** constructor */
		FreeCameraComponent(int in_player_index = 0) :
			player_index(in_player_index) {}

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;

	protected:

		/** the player index to follow */
		int player_index = 0;
	};

}; // namespace death
