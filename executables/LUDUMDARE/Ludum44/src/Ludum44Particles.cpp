#pragma once


#include "Ludum44Particles.h"
#include "Ludum44Game.h"
#include "Ludum44GameInstance.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>

chaos::GPUVertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<VertexBase>)
{
	chaos::GPUVertexDeclaration result;
	result.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT2);
	result.Push(chaos::SEMANTIC_TEXCOORD, 0, chaos::TYPE_FLOAT3);
	result.Push(chaos::SEMANTIC_COLOR, 0, chaos::TYPE_FLOAT4);
	return result;
}

// ===========================================================================
// ParticlePlayerTrait
// ===========================================================================

size_t ParticlePlayerTrait::ParticleToVertices(ParticlePlayer const * p, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const
{
	size_t result = chaos::ParticleDefault::ParticleTrait::ParticleToVertices(p, vertices, vertices_per_particle);
	return result;
}


bool ParticlePlayerTrait::UpdateParticle(float delta_time, ParticlePlayer * particle, LayerTrait const * layer_trait) const
{
	particle->bounding_box.position += delta_time * particle->velocity;


	return false;
}









// ===========================================================================
// Object particle system
// ===========================================================================


bool ParticleLifeTrait::UpdateParticle(float delta_time, ParticleLife * particle, LayerTrait const * layer_trait) const
{

	return false;
}

size_t ParticleLifeTrait::ParticleToVertices(ParticleLife const * particle, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const
{
	return chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, vertices, vertices_per_particle);
}





