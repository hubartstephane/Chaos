#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 
#include <chaos/GPUVertexDeclaration.h> 
#include <chaos/Class.h> 


#include <death/TM.h> 


static constexpr float DANGER_RADIUS_RATIO = 0.3f;
static constexpr float WAKEN_RADIUS_RATIO = 1.0f;

// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

class VertexBase : public chaos::VertexDefault
{
public:

	glm::vec2 attraction_position;
	glm::vec2 particle_center;
};

void GetTypedVertexDeclaration(chaos::GPUVertexDeclaration * result, boost::mpl::identity<VertexBase>);

class ParticleBase : public death::TMParticle
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
	float attraction_force = 0.0f;
	float repulsion_force = 0.0f;
	float tangent_force = 0.0f;

	bool reversed = false;
	bool world_limits = false;
};


// ===========================================================================
// ParticleEnemy
// ===========================================================================

class ParticleEnemy : public ParticleAffector
{
public:

	glm::vec2 rotation_center;

	float rotation_radius = 0.0f;
	float rotation_alpha = 0.0f;
};

class ParticleEnemyTrait : public chaos::ParticleAllocationTrait<ParticleEnemy, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		bool dynamic_particles = true;

		bool dynamic_vertices = true;

		class LudumGame * game = nullptr;
	};

	class UpdateEnemyData
	{
	public:

	};

	bool UpdateParticle(float delta_time, ParticleEnemy & particle, UpdateEnemyData const & update_data, LayerTrait const * layer_trait) const;

    void ParticleToPrimitives(ParticleEnemy const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const;

	UpdateEnemyData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleEnemy>& particle_accessor, LayerTrait const * layer_trait) const;
};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleAffector
{
public:

	glm::vec2 acceleration = glm::vec2(0.0f, 0.0f);
	float particle_radius_factor = 2.0f; // 2.0f         // very usefull to prevent floating atoms between player and a field
	float life = 0.0f;

	bool  level_end_reached = false;
	float level_end_timer = 0.0f;
	bool  dash = false;
};

class ParticlePlayerTrait : public chaos::ParticleAllocationTrait<ParticlePlayer, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	class UpdatePlayerData
	{
	public:


		std::vector<ParticleEnemy> enemy_particles;
	};

	bool UpdateParticle(float delta_time, ParticlePlayer& particle, UpdatePlayerData const & update_data, LayerTrait const * layer_trait) const;

	UpdatePlayerData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticlePlayer> & particle_accessor, LayerTrait const * layer_trait) const;
};


// ===========================================================================
// ParticleAtom
// ===========================================================================

class ParticleAtom : public ParticleBase
{
public:

	float particle_radius_factor = 1.0f;
	float life = 0.0f;
	bool  waken_up = false;

};

class ParticleAtomTrait : public chaos::ParticleAllocationTrait<ParticleAtom, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	class UpdateAtomData
	{
	public:

		ParticlePlayer player_particle;

		float slowing_factor;
		float max_velocity;
		float world_clamp_radius;

		std::vector<ParticleEnemy> enemy_particles;
	};

	bool UpdateParticle(float delta_time, ParticleAtom& particle, UpdateAtomData const & update_data, LayerTrait const * layer_trait) const;

	UpdateAtomData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleAtom> & particle_accessor, LayerTrait const * layer_trait) const;
};

// ===========================================================================
// Object particle system
// ===========================================================================

class ParticleLife : public chaos::ParticleDefault
{

};

class ParticleLifeTrait : public chaos::ParticleAllocationTrait<ParticleLife, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	bool UpdateParticle(float delta_time, ParticleLife& particle, LayerTrait const * layer_trait) const;
};

CHAOS_REGISTER_CLASS2(ParticleBase, death::TMParticle)
CHAOS_REGISTER_CLASS2(ParticleAffector, ParticleBase)
CHAOS_REGISTER_CLASS2(ParticleLife, chaos::ParticleDefault)
CHAOS_REGISTER_CLASS2(ParticlePlayer, ParticleAffector)
CHAOS_REGISTER_CLASS2(ParticleEnemy, ParticleAffector)
CHAOS_REGISTER_CLASS2(ParticleAtom, ParticleBase)

