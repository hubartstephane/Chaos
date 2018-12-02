#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 

#include <death/TiledMapParticle.h> 

// ===========================================================================
// VertexBase and Particle
// ===========================================================================

using VertexBase = chaos::ParticleDefault::Vertex;

// shuxxx : 
//   should be that
//using ParticleBase = chaos::ParticleDefault::Particle;
//   but, due to required refactor of TiledMapSystem, replace with that

class ParticleBase : public death::TiledMap::TileParticle
{
public:

	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);


};




// ===========================================================================
// ParticleAffector
// ===========================================================================

class ParticleAffector : public ParticleBase
{
public:

	float attraction_minradius = 0.0f;
	float attraction_maxradius = 0.0f;
	float attraction_force     = 0.0f;
	float tangent_force        = 0.0f;
};


// ===========================================================================
// ParticleEnemy
// ===========================================================================

class ParticleEnemy : public ParticleAffector
{

};



class ParticleEnemyTrait : public chaos::ParticleLayerTrait<ParticleEnemy, VertexBase>
{
public:

	class UpdateEnemyData
	{
	public:

	
	};

	bool UpdateParticle(float delta_time, ParticleEnemy * particle, chaos::ParticleAllocation * allocation, UpdateEnemyData const & update_data) const;

	size_t ParticleToVertices(ParticleEnemy const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;

	UpdateEnemyData BeginUpdateParticles(float delta_time, ParticleEnemy * particles, size_t count, chaos::ParticleAllocation * allocation) const;

public:

	class LudumGame * game = nullptr;
};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleAffector
{
public:

	float particle_radius_factor = 2.0f;
	float life = 2.0f;

};

class ParticlePlayerTrait : public chaos::ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	class UpdatePlayerData
	{
	public:


		std::vector<ParticleEnemy> enemy_particles;
	};


	bool UpdateParticle(float delta_time, ParticlePlayer * particle, chaos::ParticleAllocation * allocation, UpdatePlayerData const & update_data) const;

	size_t ParticleToVertices(ParticlePlayer const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;

	UpdatePlayerData BeginUpdateParticles(float delta_time, ParticlePlayer * particles, size_t count, chaos::ParticleAllocation * allocation) const;

public:

	class LudumGame * game = nullptr;
};


// ===========================================================================
// ParticleAtom
// ===========================================================================

class ParticleAtom : public ParticleBase
{
public:

	float particle_radius_factor = 1.0f;
	float life = 2.0f;
	bool  waken_up = false;

};

class ParticleAtomTrait : public chaos::ParticleLayerTrait<ParticleAtom, VertexBase>
{
public:

	class UpdateAtomData
	{
	public:

		ParticlePlayer player_particle;

		float particle_slowing_factor;
		float particle_max_velocity;
		float world_clamp_radius;

		std::vector<ParticleEnemy> enemy_particles;
	};

	bool UpdateParticle(float delta_time, ParticleAtom * particle, chaos::ParticleAllocation * allocation, UpdateAtomData const & update_data) const;

	size_t ParticleToVertices(ParticleAtom const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;

	UpdateAtomData BeginUpdateParticles(float delta_time, ParticleAtom * particles, size_t count, chaos::ParticleAllocation * allocation) const;

public:

	class LudumGame * game = nullptr;
};






// ===========================================================================
// Object particle system
// ===========================================================================


class ParticleLifeTrait : public chaos::ParticleLayerTrait<chaos::ParticleDefault::Particle, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, chaos::ParticleDefault::Particle * particle, chaos::ParticleAllocation * allocation) const;

	size_t ParticleToVertices(chaos::ParticleDefault::Particle const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;

public:

	class LudumGame * game = nullptr;
};


