#pragma once

#include <chaos/Chaos.h>

#include "Ludum47Game.h"
#include "Ludum47Level.h"

// =================================================
// LudumCameraComponent
// =================================================

class LudumCameraComponent : public chaos::CameraComponent
{
	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumCameraComponent, chaos::CameraComponent);

protected:

	virtual bool DoTick(float delta_time) override;

	virtual chaos::box2 ApplyModifier(chaos::box2 const& src) const override;

	float max_zoom = 2.0f;
	float min_zoom = 1.0f;
	float zoom = 1.0f;
	float zoom_increase = 0.2f;
	float zoom_decrease = 0.1f;
};


// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public chaos::TMLevelInstance
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumLevelInstance, chaos::TMLevelInstance);

	LudumLevelInstance();

	glm::ivec2 GetPlayerRacePosition(LudumPlayer const* player) const;

	void OnOpponentArrived();

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
	/** override */
	virtual bool Initialize(chaos::Game * in_game, chaos::Level * in_level) override;
	/** override */
	virtual void CreateCameras() override;

	/** override */
	virtual bool IsPlayerDead(chaos::Player* player) override;

	virtual bool CheckLevelCompletion() const override;

	virtual bool CanCompleteLevel() const override;


	





public:

	chaos::shared_ptr<LudumRoad> road;

	/** pointer on game */
	class LudumGame * game = nullptr;


	float effective_start_timer = 5.0f;

	size_t opponent_arrived_count = 0;
	size_t opponent_count = 0;

	float lost_timer = -1.0f;




	mutable float completion_timer = -1.0f; // shu47 : due to Checkcomplete constness !!
	float completion_delay = 5.0f;

};
