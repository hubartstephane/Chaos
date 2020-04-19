#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 
#include <chaos/GPUVertexDeclaration.h> 
#include <chaos/BitmapAtlas.h> 

#include <death/TiledMapParticle.h> 
#include <death/TiledMapLevel.h> 


// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

using VertexBase = chaos::ParticleDefault::Vertex;

class ParticleBase : public  death::TiledMapParticle
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

	std::vector<class SoulTriggerObject*> soul_triggers;

	death::TiledMapLayerInstance * fire_layer_instance = nullptr;

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

	bool UpdateParticle(float delta_time, ParticleSoul* particle, ParticleSoulUpdateData& update_data, LayerTrait const* layer_trait) const;

	void ParticleToPrimitives(ParticleSoul const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const;
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

	bool UpdateParticle(float delta_time, ParticleFire* particle, ParticleFireUpdateData& update_data, LayerTrait const* layer_trait) const;

	void ParticleToPrimitives(ParticleFire const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const;
};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleBase
{
public:

	int frame_index = 0;
	bool horizontal_flip = false;
	float animation_timer = 0.0f;


};

class ParticlePlayerTrait : public chaos::ParticleAllocationTrait<ParticlePlayer, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	bool UpdateParticle(float delta_time, ParticlePlayer * particle, LayerTrait const * layer_trait) const;

    void ParticleToPrimitives(ParticlePlayer const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const;
};


// ===========================================================================
// ParticleBlood
// ===========================================================================

class ParticleBlood : public ParticleBase
{
public:

	glm::vec2 acceleration = glm::vec2(0.0f, 0.0f);


	int frame_index = 0;
	float animation_timer = 0.0f;

};

class ParticleBloodTrait : public chaos::ParticleAllocationTrait<ParticleBlood, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame* game = nullptr;
	};

	bool UpdateParticle(float delta_time, ParticleBlood* particle, LayerTrait const* layer_trait) const;

	void ParticleToPrimitives(ParticleBlood const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const;
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

	bool UpdateParticle(float delta_time, ParticleBurnedSoul* particle, LayerTrait const* layer_trait) const;

	void ParticleToPrimitives(ParticleBurnedSoul const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const;
};




