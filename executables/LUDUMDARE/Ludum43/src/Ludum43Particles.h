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
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleBase
{

};

class ParticlePlayerTrait : public chaos::ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticlePlayer * particle, chaos::ParticleAllocation * allocation) const;

	size_t ParticleToVertices(ParticlePlayer const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;

public:

	class LudumGame * game = nullptr;
};




// ===========================================================================
// ParticleEnemy
// ===========================================================================

class ParticleEnemy : public ParticleBase
{

};



class ParticleEnemyTrait : public chaos::ParticleLayerTrait<ParticleEnemy, VertexBase>
{
public:

	class UpdateEnemyData
	{
	public:

		ParticlePlayer player_particle;
		float max_particle_velocity;
	
	};

	bool UpdateParticle(float delta_time, ParticleEnemy * particle, chaos::ParticleAllocation * allocation, UpdateEnemyData update_data) const;

	size_t ParticleToVertices(ParticleEnemy const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;

	UpdateEnemyData BeginUpdateParticles(float delta_time, ParticleEnemy * particles, size_t count, chaos::ParticleAllocation * allocation) const;

public:

	class LudumGame * game = nullptr;
};

// ===========================================================================
// ParticleAtom
// ===========================================================================

class ParticleAtom : public ParticleBase
{

};

class ParticleAtomTrait : public chaos::ParticleLayerTrait<ParticleAtom, VertexBase>
{
public:

	class UpdateAtomData
	{
	public:

		ParticlePlayer player_particle;
		float player_attraction_minradius;
		float player_attraction_maxradius;
		float player_attraction_force;
		float tangent_force;
		float particle_slowing_factor;
		float enemy_attraction_radius;
		float particle_max_velocity;

		std::vector<ParticleEnemy> enemy_particles;
	};

	bool UpdateParticle(float delta_time, ParticleAtom * particle, chaos::ParticleAllocation * allocation, UpdateAtomData update_data) const;

	size_t ParticleToVertices(ParticleAtom const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;

	UpdateAtomData BeginUpdateParticles(float delta_time, ParticleAtom * particles, size_t count, chaos::ParticleAllocation * allocation) const;

public:

	class LudumGame * game = nullptr;
};
