#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 
#include <chaos/PrimitiveOutput.h> 

#include <death/Game.h> 

// ===========================================================================
// VertexBase and Particle
// ===========================================================================

using VertexBase = chaos::ParticleDefault::Vertex;

using ParticleObject = chaos::ParticleDefault::Particle;

// ===========================================================================
// Object particle system
// ===========================================================================


class ParticleObjectTrait : public chaos::ParticleAllocationTrait<ParticleObject, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleObject & particle) const;
};


// ===========================================================================
// Life particle system
// ===========================================================================

class ParticleLifeObjectTrait : public ParticleObjectTrait
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};


	// called once for the whole allocation
	int BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleObject> & particle_accessor, LayerTrait const * layer_trait) const;
    // called for every particles
    bool UpdateParticle(float delta_time, ParticleObject& particle, int extra_param, LayerTrait const* layer_trait) const;
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

class ParticleBrickTrait : public chaos::ParticleAllocationTrait<ParticleBrick, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	bool UpdateParticle(float delta_time, ParticleBrick& particle, LayerTrait const * layer_trait) const;

    void ParticleToPrimitives(ParticleBrick const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const;
};

// ===========================================================================
// Movable particle system
// ===========================================================================

class ParticleMovableObject : public ParticleObject
{
public:

	float delay_before_move = 0.0f;
	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
	float damage_done_since_last_bounce = 0.0f;
};

class ParticleMovableObjectTrait : public chaos::ParticleAllocationTrait<ParticleMovableObject, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	bool UpdateParticle(float delta_time, ParticleMovableObject& particle, LayerTrait const * layer_trait) const;

    void ParticleToPrimitives(ParticleMovableObject const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const;

	void UpdateParticleVelocityFromCollision(glm::vec2 const & old_position, glm::vec2 const & new_position, glm::vec2 & velocity) const;

	glm::vec2 RestrictParticleVelocityToAngle(glm::vec2 const & v, LayerTrait const * layer_trait) const;
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

class ParticleChallengeTrait : public chaos::ParticleAllocationTrait<ParticleChallenge, VertexBase>
{
public:

    void ParticleToPrimitives(ParticleChallenge const& particle, chaos::QuadOutput<VertexBase>& output) const;
};



