#pragma once

#include "chaos/Chaos.h"

#include "Ludum50Particles.h"
#include "Ludum50Game.h"
#include "Ludum50GameInstance.h"
#include "Ludum50Player.h"
#include "Ludum50LevelInstance.h"
#include "Ludum50Level.h"
#include "Ludum50PlayerDisplacementComponent.h"

// ===========================================================================
// ParticlePlayerLayerTrait
// ===========================================================================

bool ParticlePlayerLayerTrait::UpdateParticle(float delta_time, ParticlePlayer & particle) const
{
	

	return false;
}

void ParticlePlayerLayerTrait::ParticleToPrimitives(ParticlePlayer const& particle, PrimitiveOutput<VertexDefault>& output) const
{


	chaos::ParticleToPrimitives(particle, output);
}
