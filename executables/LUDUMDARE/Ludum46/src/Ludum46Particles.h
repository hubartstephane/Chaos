#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 
#include <chaos/GPUVertexDeclaration.h> 
#include <chaos/BitmapAtlas.h> 
#include <chaos/Class.h> 

#include <death/TM.h> 



// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

using VertexBase = chaos::VertexDefault;

class ParticleBase : public  death::TMParticle
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

	death::TMLayerInstance * fire_layer_instance = nullptr;

	class LudumLevelInstance* ludum_level_instance = nullptr;

};

class ParticleSoul : public ParticleBase
{
public:

	float health = 1.0f;


};

class ParticleSoulTrait : public chaos::ParticleAllocationTrait<ParticleSoul, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame* game = nullptr;
	};

	ParticleSoulUpdateData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleSoul>& particle_accessor, LayerTrait const* layer_trait) const;

	bool UpdateParticle(float delta_time, ParticleSoul & particle, ParticleSoulUpdateData& update_data, LayerTrait const* layer_trait) const;
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

class ParticleFireTrait : public chaos::ParticleAllocationTrait<ParticleFire, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame* game = nullptr;
	};

	ParticleFireUpdateData BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleFire>& particle_accessor, LayerTrait const* layer_trait) const;

	bool UpdateParticle(float delta_time, ParticleFire & particle, ParticleFireUpdateData& update_data) const;

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

class ParticleAnimatedTrait : public chaos::ParticleAllocationTrait<ParticleAnimated, VertexBase>
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


class ParticlePlayerTrait : public chaos::ParticleAllocationTrait<ParticlePlayer, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	bool UpdateParticle(float delta_time, ParticlePlayer & particle, LayerTrait const * layer_trait) const;

	void UpdateRenderingStates(chaos::GPURenderer* renderer, bool begin) const
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
};




// ===========================================================================
// ParticleBlood
// ===========================================================================

class ParticleBlood : public ParticleAnimated
{
public:

	glm::vec2 acceleration = glm::vec2(0.0f, 0.0f);

};




class ParticleBloodTrait : public chaos::ParticleAllocationTrait<ParticleBlood, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame* game = nullptr;
	};

	bool UpdateParticle(float delta_time, ParticleBlood & particle) const;

	void ParticleToPrimitives(ParticleBlood const& particle, chaos::QuadOutput<VertexBase>& output) const;
};

// ===========================================================================
// ParticleBurnedSoul
// ===========================================================================

class ParticleBurnedSoul : public ParticleBlood
{
public:

	float offset_t = 0.0f;

};

class ParticleBurnedSoulTrait : public chaos::ParticleAllocationTrait<ParticleBurnedSoul, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame* game = nullptr;
	};

	int BeginParticlesToPrimitives(chaos::ParticleConstAccessor<ParticleBurnedSoul>& accessor, LayerTrait const* layer_trait) const;

	bool UpdateParticle(float delta_time, ParticleBurnedSoul & particle) const;

	void ParticleToPrimitives(ParticleBurnedSoul const& particle, chaos::TrianglePairOutput<VertexBase>& output, int useless, LayerTrait const* layer_trait) const;
};

CHAOS_REGISTER_CLASS2(ParticleBase, death::TMParticle);
CHAOS_REGISTER_CLASS2(ParticleAnimated, ParticleBase);
CHAOS_REGISTER_CLASS2(ParticlePlayer, ParticleAnimated);
CHAOS_REGISTER_CLASS2(ParticleFire, ParticleBase);
CHAOS_REGISTER_CLASS2(ParticleSoul, ParticleBase);
CHAOS_REGISTER_CLASS2(ParticleBlood, ParticleAnimated);
CHAOS_REGISTER_CLASS2(ParticleBurnedSoul, ParticleBlood);

