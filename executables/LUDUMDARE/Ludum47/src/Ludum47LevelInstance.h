#pragma once

#include "Ludum47PCH.h"
#include "Ludum47Game.h"
#include "Ludum47Level.h"

// =================================================
// LudumCameraComponent
// =================================================

class LudumCameraComponent : public chaos::CameraComponent
{
	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumCameraComponent, chaos::CameraComponent);

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

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumLevelInstance, chaos::TMLevelInstance);

	LudumLevelInstance();

	glm::ivec2 GetPlayerRacePosition(LudumPlayer const* player) const;

	void OnOpponentArrived();

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
	/** override */
	virtual bool Initialize(chaos::Game * in_game, chaos::Level * in_level) override;
	/** override */
	virtual void CreateCameraComponents(chaos::Camera* camera, chaos::TMCameraTemplate* camera_template) override;

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
