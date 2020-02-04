#pragma once


#include "LudumTestParticles.h"
#include "LudumTestGame.h"
#include "LudumTestGameInstance.h"
#include "LudumTestPlayer.h"
#include "LudumTestLevelInstance.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>
#include <chaos/ParticleTools.h>

#include <death/SoundContext.h>

// ===========================================================================
// ParticlePlayerTrait
// ===========================================================================

void ParticlePlayerTrait::ParticleToPrimitives(ParticlePlayer const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives(particle, output);
}

void ParticlePlayerTrait::ParticleToPrimitives(ParticlePlayer const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives(particle, output);
}

bool ParticlePlayerTrait::UpdateParticle(float delta_time, ParticlePlayer * particle, LayerTrait const * layer_trait) const
{

	return false;
}


