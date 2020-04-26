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

	bool UpdateParticle(float delta_time, ParticleObject& particle) const;
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
    void ParticleToPrimitives(ParticleBrick const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const;
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
    void ParticleToPrimitives(ParticleMovableObject const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const;

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
    void ParticleToPrimitives(ParticleChallenge const& particle, chaos::TrianglePairOutput<VertexBase>& output) const;
};



