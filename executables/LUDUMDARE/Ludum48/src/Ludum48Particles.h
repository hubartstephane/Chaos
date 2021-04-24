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
	Player = 5
};


class GameObjectParticle : public ParticleBase
{
public:

	GameObjectType type = GameObjectType::None;


	/** an offset in [-1..1] relative to the cell the particle belongs to (unit is tile_size) */
	glm::vec2 offset = { 0.0f, 0.0f };
	/** timer before moving */
	float fall_timer = -1.0f;


};

class ParticleGameObjectLayerTrait : public chaos::ParticleLayerTrait<GameObjectParticle, VertexBase>
{
public:

	void ParticleToPrimitives(GameObjectParticle const& particle, chaos::PrimitiveOutput<VertexBase>& output) const;

public:

	class LudumGame* game = nullptr;

	glm::vec2 tile_size = { 0.0f , 0.0f };
};



// ===========================================================================
// ParticleAnimated
// ===========================================================================

class ParticleAnimated : public ParticleBase
{

public:

	//bool UpdateParticle(float delta_time);

public:
	int frame_index = 0;
	float animation_timer = 0.0f;
};

bool UpdateParticle(float delta_time, ParticleAnimated& particle);

class ParticleAnimatedLayerTrait : public chaos::ParticleLayerTrait<ParticleAnimated, VertexBase>
{
public:

#if 0
	static bool UpdateParticle(float delta_time, ParticleAnimated & particle);
#endif

};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleAnimated
{
public:


};


class ParticlePlayerLayerTrait : public chaos::ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticlePlayer & particle) const;

public:

	class LudumGame* game = nullptr;
};

CHAOS_REGISTER_CLASS2(ParticleBase, chaos::TMParticle);
CHAOS_REGISTER_CLASS2(GameObjectParticle, ParticleBase);
CHAOS_REGISTER_CLASS2(ParticleAnimated, ParticleBase);
CHAOS_REGISTER_CLASS2(ParticlePlayer, ParticleAnimated);

