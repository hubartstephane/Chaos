#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 

#include <death/Game.h> 

// ===========================================================================
// VertexBase and Particle
// ===========================================================================

using VertexBase = chaos::ParticleDefault::Vertex;

using ParticleObject = chaos::ParticleDefault::Particle;

// ===========================================================================
// Object particle system
// ===========================================================================


class ParticleObjectTrait : public chaos::ParticleLayerTrait<ParticleObject, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleObject * particle, chaos::ParticleAllocation * allocation) const;

	size_t ParticleToVertices(ParticleObject const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;
};


// ===========================================================================
// Life particle system
// ===========================================================================

class ParticleLifeObjectTrait : public ParticleObjectTrait
{
public:

	class per_allocation_data
	{
	public:

		int a = 7;
		float b = 6.0f;

	};

	// called once for the whole allocation
	int BeginUpdateParticles(float delta_time, ParticleObject * particles, size_t count, chaos::ParticleAllocation * allocation, per_allocation_data const & allocation_data) const;
	// called once for the whole allocation
	int BeginParticlesToVertices(ParticleObject const * particles, size_t count, chaos::ParticleAllocation * allocation, per_allocation_data const & allocation_data) const;

	// called for every particles
	bool UpdateParticle(float delta_time, ParticleObject * particle, chaos::ParticleAllocation * allocation, per_allocation_data const & allocation_data, int extra_param) const;
	// called for every particles
	size_t ParticleToVertices(ParticleObject const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation, per_allocation_data const & allocation_data, int extra_param) const;

	class LudumGame * game = nullptr;
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

class ParticleBrickTrait : public chaos::ParticleLayerTrait<ParticleBrick, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleBrick * particle, chaos::ParticleAllocation * allocation) const;

	size_t ParticleToVertices(ParticleBrick const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;

	class LudumGame * game = nullptr;
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

class ParticleMovableObjectTrait : public chaos::ParticleLayerTrait<ParticleMovableObject, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleMovableObject * particle, chaos::ParticleAllocation * allocation) const;

	size_t ParticleToVertices(ParticleMovableObject const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;

	void UpdateParticleVelocityFromCollision(chaos::box2 const & ball_box, chaos::box2 const & new_ball_box, glm::vec2 & velocity) const;

	glm::vec2 RestrictParticleVelocityToAngle(glm::vec2 const & v) const;

	class LudumGame * game = nullptr;
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

class ParticleChallengeTrait : public chaos::ParticleLayerTrait<ParticleChallenge, VertexBase>
{
public:

	size_t ParticleToVertices(ParticleChallenge const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;
};



