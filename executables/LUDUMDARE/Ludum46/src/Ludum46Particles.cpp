#pragma once


#include "Ludum46Particles.h"
#include "Ludum46Game.h"
#include "Ludum46GameInstance.h"
#include "Ludum46Player.h"
#include "Ludum46LevelInstance.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>
#include <chaos/ParticleTools.h>

#include <death/SoundContext.h>



// ===========================================================================
// ParticleSoulTrait
// ===========================================================================

void ParticleSoulTrait::ParticleToPrimitives(ParticleSoul const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
	chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives(particle, output);
}

bool ParticleSoulTrait::UpdateParticle(float delta_time, ParticleSoul* particle, LayerTrait const* layer_trait) const
{

	return false;
}




// ===========================================================================
// ParticleFireTrait
// ===========================================================================

void ParticleFireTrait::ParticleToPrimitives(ParticleFire const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
	chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives(particle, output);
}

bool ParticleFireTrait::UpdateParticle(float delta_time, ParticleFire* particle, LayerTrait const* layer_trait) const
{
	particle->bounding_box.position += delta_time * particle->velocity;

	return false;
}






// ===========================================================================
// ParticlePlayerTrait
// ===========================================================================

void ParticlePlayerTrait::ParticleToPrimitives(ParticlePlayer const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives(particle, output);
}

bool ParticlePlayerTrait::UpdateParticle(float delta_time, ParticlePlayer* particle, LayerTrait const* layer_trait) const
{

	return false;
}



