#pragma once

#include <chaos/Chaos.h>

#include "Ludum48Game.h"

class GridCellInfo
{
public:

	GameObjectParticle* particle = nullptr;
};

class GridInfo
{
public:

	~GridInfo() 
	{
		delete[] cells;
	}

	glm::ivec2 size = {0, 0};

	GridCellInfo* cells = nullptr;

	glm::vec2 min_position = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
	glm::vec2 max_position = { std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };
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


	virtual int DoDisplay(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params) override;

	/** override */
	virtual bool IsPlayerDead(chaos::Player* player) override;

	virtual bool CheckLevelCompletion() const override;

	virtual bool CanCompleteLevel() const override;

	virtual uint64_t GetCollisionFlagByName(char const* name) const override;


	GridInfo const & GetGridInfo() const { return grid_info; }
	GridInfo& GetGridInfo(){ return grid_info; }

protected:

	void CollectObjects();



protected:

	/** pointer on game */
	class LudumGame * game = nullptr;


	mutable float completion_timer = -1.0f; // shu48 : due to Checkcomplete constness !!
	float completion_delay = 3.0f;


	GridInfo grid_info;

};
