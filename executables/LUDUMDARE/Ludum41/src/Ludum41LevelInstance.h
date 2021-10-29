#pragma once

#include <chaos/Chaos.h>

#include "Ludum41Particles.h"

// =================================================
// LevelInstance
// =================================================

class LudumLevelInstance : public chaos::LevelInstance
{

	friend class ParticleMovableObjectLayerTrait;

public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumLevelInstance, chaos::LevelInstance);

	/** override */
	virtual bool CheckLevelCompletion() const override;
	/** override */
	virtual bool CanCompleteLevel() const override;
	/** override */
	virtual bool Initialize(chaos::Game * in_game, chaos::Level * in_level) override;
	/** override */
	virtual bool DoTick(float delta_time) override;
	/** override */
	virtual chaos::PlayerPawn * CreatePlayerPawn(chaos::Player* player) override;

protected:

	virtual void CreateBackgroundImage() override;

	virtual void SetInGameMusic() override;

protected:

	chaos::ParticleAllocationBase * CreateBricks();
	
	/** get the bricks */
	ParticleBrick * GetBricks();
	/** get the bricks */
	ParticleBrick const * GetBricks() const;

	/** get the number of bricks */
	size_t GetBrickCount() const;	
	
public:

	/** some sprites */	
	chaos::shared_ptr<chaos::ParticleAllocationBase> bricks_allocations;	
	/** the current word */
	size_t word_index = 0;
};