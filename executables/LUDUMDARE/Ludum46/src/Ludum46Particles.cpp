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

ParticleFireUpdateData ParticleFireTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleFire>& particle_accessor, LayerTrait const* layer_trait) const
{
	ParticleFireUpdateData result;

	LudumLevelInstance * ludum_level_instance = layer_trait->game->GetLevelInstance();
	if (ludum_level_instance != nullptr)
	{
		result.level_bounding_box = ludum_level_instance->GetBoundingBox();

	}
	return result;
}

void ParticleFireTrait::ParticleToPrimitives(ParticleFire const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
	chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives(particle, output);
}

bool ParticleFireTrait::UpdateParticle(float delta_time, ParticleFire* particle, ParticleFireUpdateData & update_data, LayerTrait const* layer_trait) const
{
	particle->bounding_box.position += delta_time * particle->velocity;

	if (!chaos::Collide(particle->bounding_box, update_data.level_bounding_box))
		return true;

	if (particle->duration > 0.0f)
	{
		particle->life += delta_time;
		if (particle->life > particle->duration)
			return true;

		particle->color.a = 1.0f - (particle->life / particle->duration);
	}
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



