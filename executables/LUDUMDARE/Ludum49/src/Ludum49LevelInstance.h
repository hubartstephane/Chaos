#pragma once

#include <chaos/Chaos.h>

#include "Ludum49Game.h"
#include "Ludum49Level.h"

// =================================================
// Landscape
// =================================================

class Landscape : public chaos::TMObject
{
	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS2(Landscape, chaos::TMObject);

public:

	Landscape();

	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, chaos::TMObjectReferenceSolver& reference_solver) override;




	std::vector<glm::vec2> points;
};


// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public chaos::TMLevelInstance
{
public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS2(LudumLevelInstance, chaos::TMLevelInstance);

	LudumLevelInstance();


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

	virtual int DoDisplay(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params) override;

	





public:

	/** pointer on game */
	class LudumGame * game = nullptr;


	mutable float completion_timer = -1.0f; // shu49 : due to Checkcomplete constness !!
	float completion_delay = 5.0f;

};
