#pragma once

#include "Ludum46PCH.h"

// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

using VertexBase = chaos::VertexDefault;

class ParticleBase : public  chaos::TMParticle
{
public:

	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
	float life = 0.0f;
	float duration = 0.0f;

};




// ===========================================================================
// ParticleSoul
// ===========================================================================

class ParticleSoulUpdateData
{
public:

	chaos::box2 level_bounding_box;

	std::vector<class SoulTrigger*> soul_triggers;

	chaos::TMLayerInstance * fire_layer_instance = nullptr;

	class LudumLevelInstance* ludum_level_instance = nullptr;

};

class ParticleSoul : public ParticleBase
{
public:

	float health = 1.0f;


};

class ParticleSoulLayerTrait : public chaos::ParticleLayerTrait<ParticleSoul, VertexBase>
{
public:

	ParticleSoulUpdateData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleSoul>& particle_accessor) const;

	bool UpdateParticle(float delta_time, ParticleSoul & particle, ParticleSoulUpdateData const& update_data) const;

public:

	class LudumGame* game = nullptr;
};




// ===========================================================================
// ParticleFire
// ===========================================================================

class ParticleFireUpdateData
{
public:

	chaos::box2 level_bounding_box;

};

class ParticleFire : public ParticleBase
{
public:




};

class ParticleFireLayerTrait : public chaos::ParticleLayerTrait<ParticleFire, VertexBase>
{
public:

	ParticleFireUpdateData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleFire>& particle_accessor) const;

	bool UpdateParticle(float delta_time, ParticleFire & particle, ParticleFireUpdateData const& update_data) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// ParticleAnimated
// ===========================================================================

class ParticleAnimated : public ParticleBase
{

public:

	//bool UpdateParticle(float delta_time);

public:
	int frame_index = 0;
	float animation_timer = 0.0f;
};

bool UpdateParticle(float delta_time, ParticleAnimated& particle);

class ParticleAnimatedLayerTrait : public chaos::ParticleLayerTrait<ParticleAnimated, VertexBase>
{
public:

#if 0
	static bool UpdateParticle(float delta_time, ParticleAnimated & particle);
#endif

};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleAnimated
{
public:


};


class ParticlePlayerLayerTrait : public chaos::ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticlePlayer & particle) const;

	void UpdateRenderingStates(chaos::GPURenderContext* render_context, bool begin) const
	{
		if (begin)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
		}
	}

public:

	class LudumGame* game = nullptr;
};




// ===========================================================================
// ParticleBlood
// ===========================================================================

class ParticleBlood : public ParticleAnimated
{
public:

	glm::vec2 acceleration = glm::vec2(0.0f, 0.0f);

};




class ParticleBloodLayerTrait : public chaos::ParticleLayerTrait<ParticleBlood, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleBlood & particle) const;

	void ParticleToPrimitives(ParticleBlood const& particle, chaos::PrimitiveOutput<VertexBase>& output) const;

public:

	class LudumGame* game = nullptr;
};

// ===========================================================================
// ParticleBurnedSoul
// ===========================================================================

class ParticleBurnedSoul : public ParticleBlood
{
public:

	float offset_t = 0.0f;

};

class ParticleBurnedSoulLayerTrait : public chaos::ParticleLayerTrait<ParticleBurnedSoul, VertexBase>
{
public:

	int BeginParticlesToPrimitives(chaos::ParticleConstAccessor<ParticleBurnedSoul>& accessor) const;

	bool UpdateParticle(float delta_time, ParticleBurnedSoul & particle) const;

	void ParticleToPrimitives(ParticleBurnedSoul const& particle, chaos::PrimitiveOutput<VertexBase>& output, int useless) const;

public:

	class LudumGame* game = nullptr;
};

CHAOS_REGISTER_CLASS(ParticleBase, chaos::TMParticle);
CHAOS_REGISTER_CLASS(ParticleAnimated, ParticleBase);
CHAOS_REGISTER_CLASS(ParticlePlayer, ParticleAnimated);
CHAOS_REGISTER_CLASS(ParticleFire, ParticleBase);
CHAOS_REGISTER_CLASS(ParticleSoul, ParticleBase);
CHAOS_REGISTER_CLASS(ParticleBlood, ParticleAnimated);
CHAOS_REGISTER_CLASS(ParticleBurnedSoul, ParticleBlood);

