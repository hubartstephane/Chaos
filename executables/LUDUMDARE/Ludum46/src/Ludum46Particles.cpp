#pragma once


#include "Ludum46Particles.h"
#include "Ludum46Game.h"
#include "Ludum46GameInstance.h"
#include "Ludum46Player.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46Level.h"
#include "Ludum46PlayerDisplacementComponent.h"

#include <chaos/CollisionFramework.h>
#include <chaos/Class.h>
#include <chaos/ParticleTools.h>

#include <death/SoundContext.h>





// ===========================================================================
// ParticleSoulLayerTrait
// ===========================================================================

ParticleSoulUpdateData ParticleSoulLayerTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleSoul>& particle_accessor) const
{
	ParticleSoulUpdateData result;

	LudumLevelInstance* ludum_level_instance = game->GetLevelInstance();
	if (ludum_level_instance != nullptr)
	{
		result.level_bounding_box = ludum_level_instance->GetBoundingBox();
		result.fire_layer_instance = ludum_level_instance->FindLayerInstance("Fire");
		result.ludum_level_instance = ludum_level_instance;

		// store triggers
		death::TMLayerInstance* layer_instance = ludum_level_instance->FindLayerInstance("Objects");
		if (layer_instance != nullptr)
		{
			size_t count = layer_instance->GetObjectCount();
			for (size_t i = 0; i < count; ++i)
			{
				SoulTrigger* trigger = auto_cast(layer_instance->GetObject(i));
				if (trigger == nullptr)
					continue;
				result.soul_triggers.push_back(trigger);
			}
		}
	}
	return result;
}

bool ParticleSoulLayerTrait::UpdateParticle(float delta_time, ParticleSoul & particle, ParticleSoulUpdateData & update_data) const
{
	particle.bounding_box.position += delta_time * particle.velocity;

	// out of world
	if (!chaos::Collide(particle.bounding_box, update_data.level_bounding_box))
		return true;

	// out of health
	if (particle.health <= 0.0f)
		return true;

	// out of lifetime
	if (particle.duration > 0.0f)
	{
		particle.life += delta_time;
		if (particle.life > particle.duration)
			return true;

		particle.color.a = 1.0f - (particle.life / particle.duration);
	}

	// check fire particles

		// the fire elements
	if (update_data.fire_layer_instance != nullptr && update_data.fire_layer_instance->GetParticleLayer() != nullptr)
	{
		size_t count = update_data.fire_layer_instance->GetParticleLayer()->GetAllocationCount();
		for (size_t i = 0; i < count; ++i)
		{
			chaos::ParticleConstAccessor<ParticleFire> accessor = update_data.fire_layer_instance->GetParticleLayer()->GetAllocation(i)->GetParticleAccessor();
			for (ParticleFire const& fire_particle : accessor)
			{
				if (chaos::Collide(fire_particle.bounding_box, particle.bounding_box))
				{
					if (update_data.ludum_level_instance != nullptr)
						update_data.ludum_level_instance->SpawnBloodParticles(particle.bounding_box, 10);

					return true; // touching fire 
				}
			}
		}
	}

	// checking triggers
	for (SoulTrigger* trigger : update_data.soul_triggers)
	{
		if (chaos::Collide(trigger->GetBoundingBox(true), particle.bounding_box))
		{
			if (trigger->AddTriggerCount())
			{
				if (update_data.ludum_level_instance != nullptr)
					update_data.ludum_level_instance->SpawnBurnedSoulParticles(particle.bounding_box, 3);

				return true;
			}
		}
	}
	return false;
}




// ===========================================================================
// ParticleFireLayerTrait
// ===========================================================================

ParticleFireUpdateData ParticleFireLayerTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleFire>& particle_accessor) const
{
	ParticleFireUpdateData result;

	LudumLevelInstance * ludum_level_instance = game->GetLevelInstance();
	if (ludum_level_instance != nullptr)
	{
		result.level_bounding_box = ludum_level_instance->GetBoundingBox();

	}
	return result;
}

bool ParticleFireLayerTrait::UpdateParticle(float delta_time, ParticleFire & particle, ParticleFireUpdateData & update_data) const
{
	particle.bounding_box.position += delta_time * particle.velocity;

	if (!chaos::Collide(particle.bounding_box, update_data.level_bounding_box))
		return true;

	if (particle.duration > 0.0f)
	{
		particle.life += delta_time;
		if (particle.life > particle.duration)
			return true;

		particle.color.a = 1.0f - (particle.life / particle.duration);
	}
	return false;
}













// ===========================================================================
// STATICS
// ===========================================================================

static bool UpdateAnimatedParticleTexcoords(ParticleAnimated & particle) // return whether we successfully found it
{
	if (particle.bitmap_info != nullptr && particle.bitmap_info->HasAnimation())
	{
		chaos::BitmapAtlas::BitmapLayout layout = particle.bitmap_info->GetAnimationLayoutFromTime(particle.animation_timer);
		if (!layout.IsValid())
			return false;
		particle.texcoords = layout.GetTexcoords();
	}
	return true;
}

// ===========================================================================
// ParticleAnimatedLayerTrait
// ===========================================================================


bool UpdateParticle(float delta_time, ParticleAnimated & particle)
{
	particle.animation_timer += delta_time;

	// destroy the particles ? 
	if (!UpdateAnimatedParticleTexcoords(particle))
		return true;

	return false;
}
#if 0
bool ParticleAnimated::UpdateParticle(float delta_time)
{
	animation_timer += delta_time;

	// destroy the particles ? 
	if (!UpdateAnimatedParticleTexcoords(*this))
		return true;

	return false;
}


bool ParticleAnimatedLayerTrait::UpdateParticle(float delta_time, ParticleAnimated & particle)
{
	particle.animation_timer += delta_time;

	// destroy the particles ? 
	if (!UpdateAnimatedParticleTexcoords(particle))
		return true;

	return false;
}
#endif

// ===========================================================================
// ParticleBloodLayerTrait
// ===========================================================================

static bool DoUpdateBloodParticle(float delta_time, ParticleAnimated & particle)
{
	particle.bounding_box.position += delta_time * particle.velocity;

	if (particle.duration > 0.0f)
	{
		particle.life += delta_time;
		if (particle.life > particle.duration)
			return true;

		particle.color.a = 1.0f - (particle.life / particle.duration);
	}
	particle.animation_timer += delta_time;

	// destroy the particles ? 
	if (!UpdateAnimatedParticleTexcoords(particle))
		return true;

	return false;
}

// ===========================================================================
// ParticleBloodLayerTrait
// ===========================================================================

void ParticleBloodLayerTrait::ParticleToPrimitives(ParticleBlood const& particle, chaos::QuadOutput<VertexBase>& output) const
{
	ParticleBlood other = particle;
	other.bounding_box.half_size *= 1.0f + (other.life / other.duration);

	ParticleToPrimitive(other, output.AddPrimitive());
}

bool ParticleBloodLayerTrait::UpdateParticle(float delta_time, ParticleBlood & particle) const
{
	if (DoUpdateBloodParticle(delta_time, particle))
		return true;
	particle.velocity += delta_time * particle.acceleration;

	return false;
}


// ===========================================================================
// ParticleBurnedSoulLayerTrait
// ===========================================================================

void ParticleBurnedSoulLayerTrait::ParticleToPrimitives(ParticleBurnedSoul const& particle, chaos::TrianglePairOutput<VertexBase>& output, int useless) const
{
	ParticleBurnedSoul other = particle;
	other.bounding_box.position.x += 50.0f * std::sin(other.offset_t);

	ParticleToPrimitive(other, output.AddPrimitive());
}

bool ParticleBurnedSoulLayerTrait::UpdateParticle(float delta_time, ParticleBurnedSoul & particle) const
{
	if (DoUpdateBloodParticle(delta_time, particle))
		return true;
	particle.velocity += delta_time * particle.acceleration;
	particle.offset_t += delta_time;

	return false;
}

int ParticleBurnedSoulLayerTrait::BeginParticlesToPrimitives(chaos::ParticleConstAccessor<ParticleBurnedSoul>& accessor) const
{
	return 666;
}


// ===========================================================================
// ParticlePlayerLayerTrait
// ===========================================================================

bool ParticlePlayerLayerTrait::UpdateParticle(float delta_time, ParticlePlayer & particle) const
{



#if 0

	LudumPlayerDisplacementComponent* displacement_component = game->GetPlayerDisplacementComponent(0);
	if (displacement_component != nullptr)
	{
		glm::vec2 pawn_velocity = displacement_component->GetPawnVelocity();

		particle.flags &= ~chaos::ParticleFlags::TEXTURE_HORIZONTAL_FLIP;

		if (std::abs(pawn_velocity.x) < 1.0f)
		{
			particle.frame_index = 0;
		}
		else
		{
			if (std::abs(pawn_velocity.x) < 12.0f)
			{
				particle.frame_index = 3;

			}
			else
			{
				float max_pawn_velocity = displacement_component->GetDisplacementInfo().max_pawn_velocity.x;

				float speed_factor = 8.0f;
				if (max_pawn_velocity > 0.0f)
					speed_factor = std::max(1.0f, speed_factor * std::abs(pawn_velocity.x) / max_pawn_velocity);

				particle.animation_timer += speed_factor * delta_time;

				particle.frame_index = 1 + (int)std::fmodf(particle.animation_timer, 2.0f);
			}

			if (pawn_velocity.x < 0.0f)
				particle.flags |= chaos::ParticleFlags::TEXTURE_HORIZONTAL_FLIP;
		}
	}
#endif

	if (particle.bitmap_info != nullptr && particle.bitmap_info->HasAnimation())
	{
		chaos::BitmapAtlas::BitmapLayout layout = particle.bitmap_info->GetAnimationLayout(particle.frame_index);
		if (!layout.IsValid())
			return true; // destroy the particle

		particle.texcoords = layout.GetTexcoords();
	}
	return false;
}



