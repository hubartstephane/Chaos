#pragma once

#include "chaos/Chaos.h"

#include "Ludum49Particles.h"
#include "Ludum49Game.h"
#include "Ludum49GameInstance.h"
#include "Ludum49Player.h"
#include "Ludum49LevelInstance.h"
#include "Ludum49Level.h"
#include "Ludum49PlayerDisplacementComponent.h"

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
