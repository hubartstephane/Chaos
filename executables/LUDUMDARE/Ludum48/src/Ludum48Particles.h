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
	Stone = 1,
	Diamond = 2,
	Wall = 3,
	Foam = 4,
	Player = 5
};


class GameObjectParticle : public ParticleBase
{
public:

	GameObjectType type = GameObjectType::None;


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

