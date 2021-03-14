#pragma once

#include <chaos/Chaos.h>

static constexpr float DANGER_RADIUS_RATIO = 0.3f;
static constexpr float WAKEN_RADIUS_RATIO  = 1.0f;

// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

using VertexBase = chaos::VertexDefault;

class ParticleBase : public chaos::TMParticle
{
public:

	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
};

// =====================================
// PowerUpZoneParticleLayerTrait
// =====================================

class ParticlePowerUpZone : public chaos::TMParticle
{
public:

	ParticlePowerUpZone() 
	{
	}

};

class VertexPowerUpZone : public VertexBase
{
public:
	// texcoord(1) is used for bottom_left corner or sprite in atlas.
	// texcoord2   is used for top_left corner or sprite in atlas.
	// texcoord3   is use to indicate in [0..1] where we are
	glm::vec3 texcoord2; 
	glm::vec2 texcoord3;
};

void GetTypedVertexDeclaration(chaos::GPUVertexDeclaration * result, boost::mpl::identity<VertexPowerUpZone>);


class PowerUpZoneParticleLayerTrait : public chaos::ParticleLayerTrait<ParticlePowerUpZone, VertexPowerUpZone> 
{
public:

    void ParticleToPrimitives(chaos::TMParticle const& particle, chaos::PrimitiveOutput<VertexPowerUpZone>& output) const;

	bool UpdateParticle(float delta_time, ParticlePowerUpZone & particle);
};

// ===========================================================================
// ParticleEnemy
// ===========================================================================

class ParticleEnemy : public ParticleBase
{
public:

	float life = 5.0f;
	float rotation_speed = 0.0f;
	int   touched_count_down = 0;
	int   score = 50;
	float damage_for_player = 0.1f;

	float fire_frequency = 0.0f;
	float current_fire_timer = 0.0f;

	bool rotation_following_player = false;

};

class ParticleEnemyUpdateData
{
public:

	chaos::box2 camera_box;
};

class ParticleEnemyLayerTrait : public chaos::ParticleLayerTrait<ParticleEnemy, VertexBase>
{
public:

	ParticleEnemyUpdateData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleEnemy>& particle_accessor) const;

	bool UpdateParticle(float delta_time, ParticleEnemy& particle, ParticleEnemyUpdateData const & update_data) const;

    void ParticleToPrimitives(ParticleEnemy const& particle, chaos::PrimitiveOutput<VertexBase>& output) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// ParticleFire
// ===========================================================================

class ParticleFire : public ParticleBase
{
public:

	bool player_ownership = true;
	bool trample = false;
	float damage = 1.0f;
	float lifetime = 10.0f;

};

class ParticleFireUpdateData
{
public:

	/** the camera box */
	chaos::box2 camera_box;
	/** all the enemies */
	std::vector<ParticleEnemy*> enemies;
	/** the main player */
	class LudumPlayer * player = nullptr;
};

class ParticleFireLayerTrait : public chaos::ParticleLayerTrait<ParticleFire, VertexBase>
{
public:

	ParticleFireUpdateData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleFire>& particle_accessor) const;

	bool UpdateParticle(float delta_time, ParticleFire& particle, ParticleFireUpdateData const & update_data) const;

    void ParticleToPrimitives(ParticleFire const& particle, chaos::PrimitiveOutput<VertexBase>& output) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// ParticleLife
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

// ===========================================================================
// ParticleLife
// ===========================================================================

class ParticleExplosion : public chaos::ParticleDefault
{

public:

	chaos::BitmapAtlas::BitmapInfo const * explosion_info = nullptr;

	float age = 0.0f;
};

class ParticleExplosionLayerTrait : public chaos::ParticleLayerTrait<ParticleExplosion, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleExplosion& particle) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleBase
{
public:

	

};

class ParticlePlayerLayerTrait : public chaos::ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticlePlayer& particle) const;

public:

	class LudumGame* game = nullptr;
};

CHAOS_REGISTER_CLASS2(ParticleBase, chaos::TMParticle)
CHAOS_REGISTER_CLASS2(ParticlePowerUpZone, chaos::TMParticle)
CHAOS_REGISTER_CLASS2(ParticlePlayer, ParticleBase)
CHAOS_REGISTER_CLASS2(ParticleEnemy, ParticleBase)
CHAOS_REGISTER_CLASS2(ParticleFire, ParticleBase)
CHAOS_REGISTER_CLASS2(ParticleLife, chaos::ParticleDefault)
CHAOS_REGISTER_CLASS2(ParticleExplosion, chaos::ParticleDefault)


