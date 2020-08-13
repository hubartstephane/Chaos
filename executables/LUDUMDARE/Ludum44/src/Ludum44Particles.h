#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 
#include <chaos/GPUVertexDeclaration.h> 
#include <chaos/BitmapAtlas.h> 
#include <chaos/Class.h> 

#include <death/TiledMapParticle.h> 


static constexpr float DANGER_RADIUS_RATIO = 0.3f;
static constexpr float WAKEN_RADIUS_RATIO  = 1.0f;

// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

class VertexBase : public chaos::VertexDefault
{
public:

};

void GetTypedVertexDeclaration(chaos::GPUVertexDeclaration * result, boost::mpl::identity<VertexBase>);

class ParticleBase : public death::TiledMapParticle
{
public:

	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);

};

// =====================================
// PowerUpZoneParticleTrait
// =====================================

class ParticlePowerUpZone : public death::TiledMapParticle
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


class PowerUpZoneParticleTrait : public chaos::ParticleAllocationTrait<ParticlePowerUpZone, VertexPowerUpZone, true, true> // shuxxx set to false = optimization 1
{
public:

    void ParticleToPrimitives(death::TiledMapParticle const& particle, chaos::QuadOutput<VertexPowerUpZone>& output) const;

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

class ParticleEnemyTrait : public chaos::ParticleAllocationTrait<ParticleEnemy, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	ParticleEnemyUpdateData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleEnemy>& particle_accessor, LayerTrait const * layer_trait) const;

	bool UpdateParticle(float delta_time, ParticleEnemy& particle, ParticleEnemyUpdateData const & update_data, LayerTrait const * layer_trait) const;

    void ParticleToPrimitives(ParticleEnemy const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const;
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

class ParticleFireTrait : public chaos::ParticleAllocationTrait<ParticleFire, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	ParticleFireUpdateData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleFire>& particle_accessor, LayerTrait const * layer_trait) const;

	bool UpdateParticle(float delta_time, ParticleFire& particle, ParticleFireUpdateData const & update_data, LayerTrait const * layer_trait) const;

    void ParticleToPrimitives(ParticleFire const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const;
};

// ===========================================================================
// ParticleLife
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

// ===========================================================================
// ParticleLife
// ===========================================================================

class ParticleExplosion : public chaos::ParticleDefault
{

public:

	chaos::BitmapAtlas::BitmapInfo const * explosion_info = nullptr;

	float age = 0.0f;
};

class ParticleExplosionTrait : public chaos::ParticleAllocationTrait<ParticleExplosion, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	bool UpdateParticle(float delta_time, ParticleExplosion& particle, LayerTrait const * layer_trait) const;
};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleBase
{
public:

	

};

class ParticlePlayerTrait : public chaos::ParticleAllocationTrait<ParticlePlayer, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	bool UpdateParticle(float delta_time, ParticlePlayer& particle, LayerTrait const * layer_trait) const;
};

CHAOS_REGISTER_CLASS2(ParticleBase, death::TiledMapParticle)
CHAOS_REGISTER_CLASS2(ParticlePowerUpZone, death::TiledMapParticle)
CHAOS_REGISTER_CLASS2(ParticlePlayer, ParticleBase)
CHAOS_REGISTER_CLASS2(ParticleEnemy, ParticleBase)
CHAOS_REGISTER_CLASS2(ParticleFire, ParticleBase)
CHAOS_REGISTER_CLASS2(ParticleLife, chaos::ParticleDefault)
CHAOS_REGISTER_CLASS2(ParticleExplosion, chaos::ParticleDefault)


