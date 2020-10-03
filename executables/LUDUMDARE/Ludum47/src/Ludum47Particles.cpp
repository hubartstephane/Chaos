#pragma once


#include "Ludum47Particles.h"
#include "Ludum47Game.h"
#include "Ludum47GameInstance.h"
#include "Ludum47Player.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47Level.h"
#include "Ludum47PlayerDisplacementComponent.h"

#include <chaos/CollisionFramework.h>
#include <chaos/Class.h>
#include <chaos/ParticleTools.h>

#include <death/SoundContext.h>





// ===========================================================================
// ParticlePlayerLayerTrait
// ===========================================================================

bool ParticlePlayerLayerTrait::UpdateParticle(float delta_time, ParticlePlayer & particle) const
{
	particle.bounding_box.position += particle.velocity * glm::vec2(std::cos(particle.rotation), std::sin(particle.rotation)) * delta_time;

	return false;
}



// ===========================================================================
// ParticleOpponentLayerTrait
// ===========================================================================

bool ParticleOpponentLayerTrait::UpdateParticle(float delta_time, ParticleOpponent& particle) const
{

	

	return false;
}

