#pragma once

#include "Ludum41CustomPCH.h"

// ===========================================================================
// VertexBase and Particle
// ===========================================================================

using VertexBase = chaos::VertexDefault;

using ParticleObject = chaos::ParticleDefault;

// ===========================================================================
// Object particle system
// ===========================================================================


class ParticleObjectLayerTrait : public chaos::ParticleLayerTrait<ParticleObject, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleObject & particle) const;
};


// ===========================================================================
// Life particle system
// ===========================================================================

class ParticleLifeLayerTrait : public ParticleObjectLayerTrait
{
public:

	// called once for the whole allocation
	int BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleObject> & particle_accessor) const;
    // called for every particles
    bool UpdateParticle(float delta_time, ParticleObject& particle, int extra_param) const;

public:

	class LudumGame* game = nullptr;
};


// ===========================================================================
// Brick particle system
// ===========================================================================

class ParticleBrick : public ParticleObject
{
public:

	float starting_life  = 0.0f;
	float life           = 0.0f;
	bool  indestructible = false;
	int   special_type   = 0;
	float highlight_time = 0.0f;
};

class ParticleBrickLayerTrait : public chaos::ParticleLayerTrait<ParticleBrick, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleBrick& particle) const;

    void ParticleToPrimitives(ParticleBrick const& particle, chaos::GPUPrimitiveOutput<VertexBase>& output) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// Movable particle system
// ===========================================================================

class ParticleMovableObject : public ParticleObject
{
public:

	float delay_before_move;
	glm::vec2 velocity;
};

class ParticleMovableObjectLayerTrait : public chaos::ParticleLayerTrait<ParticleMovableObject, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleMovableObject& particle) const;

    void ParticleToPrimitives(ParticleMovableObject const& particle, chaos::GPUPrimitiveOutput<VertexBase>& output) const;

	void UpdateParticleVelocityFromCollision(glm::vec2 const & old_position, glm::vec2 const & new_position, glm::vec2 & velocity) const;

	glm::vec2 RestrictParticleVelocityToAngle(glm::vec2 const & v) const;

public:

	class LudumGame* game = nullptr;
};









// ===========================================================================
// Challenge particle system
// ===========================================================================

class ParticleChallenge : public ParticleObject
{
public:

	class LudumChallenge * challenge = nullptr;
	size_t index = 0;
};

class ParticleChallengeLayerTrait : public chaos::ParticleLayerTrait<ParticleChallenge, VertexBase>
{
public:

    void ParticleToPrimitives(ParticleChallenge const& particle, chaos::GPUPrimitiveOutput<VertexBase>& output) const;
};


CHAOS_REGISTER_CLASS(ParticleObject);
CHAOS_REGISTER_CLASS(ParticleBrick, ParticleObject);
CHAOS_REGISTER_CLASS(ParticleMovableObject, ParticleObject);
CHAOS_REGISTER_CLASS(ParticleChallenge, ParticleObject);
