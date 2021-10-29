#pragma once

#include <chaos/Chaos.h>

// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

using VertexBase = chaos::VertexDefault;

class ParticleBase : public  chaos::TMParticle
{
public:

	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);

};

// ===========================================================================
// ParticleLifeLayerTrait
// ===========================================================================

using ParticleLife = chaos::ParticleDefault;

class ParticleLifeLayerTrait : public chaos::ParticleLayerTrait<chaos::ParticleDefault, chaos::VertexDefault>
{
public:

	bool UpdateParticle(float delta_time, ParticleLife& particle) const;
};

// ===========================================================================
// ParticleBonus
// ===========================================================================


class ParticleBonus : public ParticleBase
{
public:

	

	chaos::TagType bonus_type;
};

class ParticleBonusLayerTrait : public chaos::ParticleLayerTrait<ParticleBonus, VertexBase>
{
public:

	std::vector<chaos::box2> BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleBonus>& particle_accessor) const;

	bool UpdateParticle(float delta_time, ParticleBonus& particle, std::vector<chaos::box2> const & player_boxes) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// ParticleEnemy
// ===========================================================================


class ParticleEnemy : public ParticleBase
{
public:


	float enemy_health = 0.0f;

	float enemy_damage = 10.0f;

	float touched_count_down = 0.0f;


	float  orientation = 0.0f;
	float  image_timer = 0.0f;
	int    current_frame = 0;

	int enemy_particle_count = 0;

	int         enemy_index = 0;
	float       time = 0.0f;
	chaos::box2 spawner_box;

	class EnemyPattern * pattern = nullptr;
	
};

class ParticleEnemyLayerTrait : public chaos::ParticleLayerTrait<ParticleEnemy, VertexBase>
{
public:

	std::vector<chaos::box2> BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleEnemy>& particle_accessor) const;

	bool UpdateParticle(float delta_time, ParticleEnemy& particle, std::vector<chaos::box2> const& player_boxes) const;

    void ParticleToPrimitives(ParticleEnemy const& particle, chaos::PrimitiveOutput<VertexBase>& output) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleBase
{
public:

	float  image_timer = 0.0f;
	int current_frame = 0;
	

};

class ParticlePlayerLayerTrait : public chaos::ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticlePlayer& particle) const;

    void ParticleToPrimitives(ParticlePlayer const& particle, chaos::PrimitiveOutput<VertexBase>& output) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// ParticleShroudLife
// ===========================================================================

class ParticleShroudLife : public ParticleBase
{
public:

	chaos::BitmapAtlas::BitmapInfo const * bitmap_info = nullptr;

};

class ParticleShroudLifeTrait : public chaos::ParticleLayerTrait<ParticleShroudLife, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleShroudLife& particle) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticleFire : public ParticleBase
{
public:

	bool  player_ownership = true;
	bool  trample = false;
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

	ParticleFireUpdateData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleFire> & particle_accessor) const;

	bool UpdateParticle(float delta_time, ParticleFire& particle, ParticleFireUpdateData const & update_data) const;

    void ParticleToPrimitives(ParticleFire const& particle, chaos::PrimitiveOutput<VertexBase>& output) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// ParticleExplosion
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

	bool UpdateParticle(float delta_time, ParticleExplosion & particle) const;

public:

	class LudumGame* game = nullptr;
};

CHAOS_REGISTER_CLASS(ParticleBase, chaos::TMParticle)
CHAOS_REGISTER_CLASS(ParticlePlayer, ParticleBase)
CHAOS_REGISTER_CLASS(ParticleFire, ParticleBase)
CHAOS_REGISTER_CLASS(ParticleBonus, ParticleBase)
CHAOS_REGISTER_CLASS(ParticleEnemy, ParticleBase)
CHAOS_REGISTER_CLASS(ParticleShroudLife, ParticleBase)
CHAOS_REGISTER_CLASS(ParticleExplosion, chaos::ParticleDefault)

