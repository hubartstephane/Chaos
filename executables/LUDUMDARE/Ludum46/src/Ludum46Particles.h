#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 
#include <chaos/GPUVertexDeclaration.h> 
#include <chaos/BitmapAtlas.h> 

#include <death/TiledMapParticle.h> 


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

};

class ParticleSoul : public ParticleBase
{
public:


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





