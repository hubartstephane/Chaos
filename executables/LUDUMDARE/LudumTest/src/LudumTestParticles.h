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
    void ParticleToPrimitives(ParticlePlayer const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const;
};





