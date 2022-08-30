#pragma once

#include "Ludum43PCH.h"

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

class ParticleBase : public chaos::TMParticle
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

class ParticleEnemyLayerTrait : public chaos::ParticleLayerTrait<ParticleEnemy, VertexBase>
{
public:

	class UpdateEnemyData
	{
	public:

	};

	bool UpdateParticle(float delta_time, ParticleEnemy & particle, UpdateEnemyData const & update_data) const;

    void ParticleToPrimitives(ParticleEnemy const& particle, chaos::PrimitiveOutput<VertexBase>& output) const;

	UpdateEnemyData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleEnemy>& particle_accessor) const;

public:

	class LudumGame* game = nullptr;
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

class ParticlePlayerLayerTrait : public chaos::ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	class UpdatePlayerData
	{
	public:


		std::vector<ParticleEnemy> enemy_particles;
	};

	bool UpdateParticle(float delta_time, ParticlePlayer& particle, UpdatePlayerData const & update_data) const;

	UpdatePlayerData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticlePlayer> & particle_accessor) const;

public:

	class LudumGame* game = nullptr;
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

class ParticleAtomLayerTrait : public chaos::ParticleLayerTrait<ParticleAtom, VertexBase>
{
public:

	class UpdateAtomData
	{
	public:

		ParticlePlayer player_particle;

		float slowing_factor;
		float max_velocity;
		float world_clamp_radius;

		std::vector<ParticleEnemy> enemy_particles;
	};

	bool UpdateParticle(float delta_time, ParticleAtom& particle, UpdateAtomData const & update_data) const;

	UpdateAtomData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleAtom> & particle_accessor) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// Object particle system
// ===========================================================================

class ParticleLife : public chaos::ParticleDefault
{

};

class ParticleLifeLayerTrait : public chaos::ParticleLayerTrait<ParticleLife, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleLife& particle) const;

public:

	class LudumGame* game = nullptr;
};

CHAOS_REGISTER_CLASS(ParticleBase, chaos::TMParticle);
CHAOS_REGISTER_CLASS(ParticleAffector, ParticleBase);
CHAOS_REGISTER_CLASS(ParticleLife, chaos::ParticleDefault);
CHAOS_REGISTER_CLASS(ParticlePlayer, ParticleAffector);
CHAOS_REGISTER_CLASS(ParticleEnemy, ParticleAffector);
CHAOS_REGISTER_CLASS(ParticleAtom, ParticleBase);

