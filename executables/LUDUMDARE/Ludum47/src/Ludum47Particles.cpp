#pragma once

#include <chaos/Chaos.h>

#include "Ludum47Particles.h"
#include "Ludum47Game.h"
#include "Ludum47GameInstance.h"
#include "Ludum47Player.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47Level.h"
#include "Ludum47PlayerDisplacementComponent.h"


// ===========================================================================
// ParticleSmoke
// ===========================================================================

bool ParticleSmokeLayerTrait::UpdateParticle(float delta_time, ParticleSmoke& particle) const
{
	particle.lifetime -= delta_time;
	if (particle.lifetime <= 0.0f)
		return true;


	particle.bounding_box.half_size = (0.1f + 0.9f * (1.0f - (particle.lifetime / particle.duration))) * glm::vec2(16.0f , 16.0f);
	particle.bounding_box.position += particle.velocity * delta_time;

	float c = 0.7f * (particle.lifetime / particle.duration);

	particle.color = glm::vec4(c, c, c, c);

	particle.rotation += particle.angular_velocity * delta_time;

	return false;
}


// ===========================================================================
// ParticlePlayerLayerTrait
// ===========================================================================

bool ParticlePlayerLayerTrait::UpdateParticle(float delta_time, ParticlePlayer & particle) const
{
	//particle.bounding_box.position += particle.velocity * glm::vec2(std::cos(particle.rotation), std::sin(particle.rotation)) * delta_time;

	return false;
}

void ParticlePlayerLayerTrait::ParticleToPrimitives(ParticlePlayer const& particle, chaos::TrianglePairOutput<chaos::VertexDefault>& output) const
{
	ParticlePlayer clamped_particle = particle;

	clamped_particle.rotation = chaos::MathTools::DegreeToRadian(

		6.0f * std::floor(chaos::MathTools::RadianToDegree(particle.rotation) / 6.0f)
	);


	chaos::ParticleToPrimitives(clamped_particle, output);
}


// ===========================================================================
// ParticleOpponentLayerTrait
// ===========================================================================

bool ParticleOpponentLayerTrait::UpdateParticle(float delta_time, ParticleOpponent& particle) const
{

	

	return false;
}

