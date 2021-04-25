#pragma once

#include <chaos/Chaos.h>

// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

using VertexBase = chaos::VertexDefault;

class ParticleBase : public  chaos::TMParticle
{
public:


};


// ===========================================================================
// GameObjectParticle
// ===========================================================================

enum class GameObjectType
{
	None = 0,
	Rock = 1,
	Diamond = 2,
	Wall = 3,
	Foam = 4,
	Player = 5,
	Blob = 6,
	Monster = 7,
};


class GameObjectParticle : public ParticleBase
{
public:

	GameObjectType type = GameObjectType::None;



	class GridCellInfo* locked_cell = nullptr;

	/** an offset in [-1..1] relative to the cell the particle belongs to (unit is tile_size) */
	glm::vec2 offset = { 0.0f, 0.0f };
	/** direction where the object want to go */
	glm::vec2 direction = { 0.0f, 0.0f };
	/** the displacement speed along the direction */
	float speed = 1.0f;


	/** direction orientation of the monster */
	int monster_direction_index = 0;



	bool destroy_particle = false;

};

class ParticleGameObjectLayerTrait : public chaos::ParticleLayerTrait<GameObjectParticle, VertexBase>
{
public:

	void ParticleToPrimitives(GameObjectParticle const& particle, chaos::PrimitiveOutput<VertexBase>& output) const;

	bool UpdateParticle(float delta_time, GameObjectParticle& particle) const;

public:

	class LudumGame* game = nullptr;

	glm::vec2 tile_size = { 0.0f , 0.0f };
};



// ===========================================================================
// ParticleAnimated
// ===========================================================================

class ParticleAnimated : public GameObjectParticle
{



public:
	int frame_index = 0;
	float animation_timer = 0.0f;
};

bool UpdateParticle(float delta_time, ParticleAnimated& particle);

class ParticleAnimatedLayerTrait : public chaos::ParticleLayerTrait<ParticleAnimated, VertexBase>
{
public:



};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleAnimated
{
public:

	ParticlePlayer()
	{
		type = GameObjectType::Player;
	}


};


class ParticlePlayerLayerTrait : public chaos::ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticlePlayer & particle) const;

	void ParticleToPrimitives(ParticlePlayer const& particle, chaos::PrimitiveOutput<VertexBase>& output) const;

public:

	class LudumGame* game = nullptr;

	glm::vec2 tile_size = { 0.0f , 0.0f };
};






bool UpdateParticlePositionInGrid(GameObjectParticle* particle, float delta_time, class GridInfo& grid_info);




CHAOS_REGISTER_CLASS2(ParticleBase, chaos::TMParticle);
CHAOS_REGISTER_CLASS2(GameObjectParticle, ParticleBase);
CHAOS_REGISTER_CLASS2(ParticleAnimated, GameObjectParticle);
CHAOS_REGISTER_CLASS2(ParticlePlayer, ParticleAnimated);

