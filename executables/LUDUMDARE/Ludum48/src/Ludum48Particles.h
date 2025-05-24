#pragma once
#include "Ludum48PCH.h"
#include "Ludum48.h"

// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

using VertexBase = VertexDefault;

class ParticleBase : public TMParticle
{
public:


};


// ===========================================================================
// SmokeParticle
// ===========================================================================

class SmokeParticle : public ParticleBase
{
public:

	glm::vec2 velocity = { 0.0f, 0.0f };
	float lifetime = 1.0f;
	float age = 0.0f;

};

class ParticleSmokeLayerTrait : public ParticleLayerTrait<SmokeParticle, VertexBase>
{
public:

	void ParticleToPrimitives(SmokeParticle const& particle, GPUPrimitiveOutput<VertexBase>& output) const;

	bool UpdateParticle(float delta_time, SmokeParticle& particle) const;

public:

	class LudumGame* game = nullptr;

	glm::vec2 tile_size = { 0.0f , 0.0f };
};



// ===========================================================================
// GateParticle
// ===========================================================================

class GateParticle : public ParticleBase
{
public:

};

class ParticleGateLayerTrait : public ParticleLayerTrait<GateParticle, VertexBase>
{
public:

	void ParticleToPrimitives(GateParticle const& particle, GPUPrimitiveOutput<VertexBase>& output) const;

	bool UpdateParticle(float delta_time, GateParticle& particle) const;

public:

	class LudumGame* game = nullptr;

	glm::vec2 tile_size = { 0.0f , 0.0f };
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
	Monster1 = 7,
	HardWall = 8,
	Monster2 = 9,
};


class GameObjectParticle : public ParticleBase
{
public:

	GameObjectType type = GameObjectType::None;

	/** an offset in [-1..1] relative to the cell the particle belongs to (unit is tile_size) */
	glm::vec2 offset = { 0.0f, 0.0f };
	/** direction where the object want to go */
	glm::vec2 direction = { 0.0f, 0.0f };
	/** the displacement speed along the direction */
	float speed = 0.0f;


	/** direction orientation of the monster */
	int monster_direction_index = 0;

	bool falling_previous_frame = false;

	bool destroy_particle = false;

};

class ParticleGameObjectLayerTrait : public ParticleLayerTrait<GameObjectParticle, VertexBase>
{
public:

	void ParticleToPrimitives(GameObjectParticle const& particle, GPUPrimitiveOutput<VertexBase>& output) const;

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


class ParticleAnimatedLayerTrait : public ParticleLayerTrait<ParticleAnimated, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleAnimated& particle) const;

	void ParticleToPrimitives(ParticleAnimated const& particle, GPUPrimitiveOutput<VertexBase>& output) const;

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


	mutable float idle_timer = 0.0f;


};


class ParticlePlayerLayerTrait : public ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticlePlayer & particle) const;

	void ParticleToPrimitives(ParticlePlayer const& particle, GPUPrimitiveOutput<VertexBase>& output) const;

public:

	class LudumGame* game = nullptr;

	glm::vec2 tile_size = { 0.0f , 0.0f };
};






bool UpdateParticlePositionInGrid(GameObjectParticle* particle, float delta_time, class GridInfo& grid_info);


CHAOS_REGISTER_CLASS(ParticleBase, TMParticle);
CHAOS_REGISTER_CLASS(GateParticle, ParticleBase);
CHAOS_REGISTER_CLASS(SmokeParticle, ParticleBase);
CHAOS_REGISTER_CLASS(GameObjectParticle, ParticleBase);
CHAOS_REGISTER_CLASS(ParticleAnimated, GameObjectParticle);
CHAOS_REGISTER_CLASS(ParticlePlayer, ParticleAnimated);


