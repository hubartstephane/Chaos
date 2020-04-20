#pragma once


#include "Ludum46Particles.h"
#include "Ludum46Game.h"
#include "Ludum46GameInstance.h"
#include "Ludum46Player.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46Level.h"
#include "Ludum46PlayerDisplacementComponent.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>
#include <chaos/ParticleTools.h>

#include <death/SoundContext.h>





// ===========================================================================
// ParticleSoulTrait
// ===========================================================================

ParticleSoulUpdateData ParticleSoulTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleSoul>& particle_accessor, LayerTrait const* layer_trait) const
{
	ParticleSoulUpdateData result;

	LudumLevelInstance* ludum_level_instance = layer_trait->game->GetLevelInstance();
	if (ludum_level_instance != nullptr)
	{
		result.level_bounding_box = ludum_level_instance->GetBoundingBox();
		result.fire_layer_instance = ludum_level_instance->FindLayerInstance("Fire");
		result.ludum_level_instance = ludum_level_instance;

		// store triggers
		death::TiledMapLayerInstance* layer_instance = ludum_level_instance->FindLayerInstance("Objects");
		if (layer_instance != nullptr)
		{
			size_t count = layer_instance->GetGeometricObjectCount();
			for (size_t i = 0; i < count; ++i)
			{
				SoulTriggerObject* trigger = auto_cast(layer_instance->GetGeometricObject(i));
				if (trigger == nullptr)
					continue;
				result.soul_triggers.push_back(trigger);
			}
		}
	}
	return result;
}

void ParticleSoulTrait::ParticleToPrimitives(ParticleSoul const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
	chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives(particle, output);
}

bool ParticleSoulTrait::UpdateParticle(float delta_time, ParticleSoul* particle, ParticleSoulUpdateData & update_data, LayerTrait const* layer_trait) const
{
	particle->bounding_box.position += delta_time * particle->velocity;

	// out of world
	if (!chaos::Collide(particle->bounding_box, update_data.level_bounding_box))
		return true;

	// out of health
	if (particle->health <= 0.0f)
		return true;

	// out of lifetime
	if (particle->duration > 0.0f)
	{
		particle->life += delta_time;
		if (particle->life > particle->duration)
			return true;

		particle->color.a = 1.0f - (particle->life / particle->duration);	
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
				if (chaos::Collide(fire_particle.bounding_box, particle->bounding_box))
				{
					if (update_data.ludum_level_instance != nullptr)
						update_data.ludum_level_instance->SpawnBloodParticles(particle->bounding_box, 10);

					return true; // touching fire 
				}
			}
		}
	}

	// checking triggers
	for (SoulTriggerObject* trigger : update_data.soul_triggers)
	{
		if (chaos::Collide(trigger->GetBoundingBox(true), particle->bounding_box))
		{
			if (trigger->AddTriggerCount())
			{
				if (update_data.ludum_level_instance != nullptr)
					update_data.ludum_level_instance->SpawnBurnedSoulParticles(particle->bounding_box, 3);

				return true;
			}
		}
	}
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
// STATICS
// ===========================================================================

static chaos::ParticleTexcoords DoGetAnimatedParticleTexcoords(ParticleAnimated const& particle, chaos::WrapMode mode)
{
	chaos::ParticleTexcoords result = particle.texcoords;

	if (particle.bitmap_info != nullptr && particle.bitmap_info->HasAnimation())
	{
		// shu46 : chaos::WrapMode::clamp not working; to fix

		chaos::BitmapAtlas::BitmapLayout layout = particle.bitmap_info->GetAnimationLayout(particle.frame_index, mode);

		result = chaos::ParticleTools::GetParticleTexcoords(layout); // shu46 : should be a member of BitmapLayout itself shouldnt it??!
		result = chaos::ParticleTools::ApplySymetriesToTexcoords(result, particle.horizontal_flip, particle.vertical_flip, particle.diagonal_flip);
	}
	return result;
}

// ===========================================================================
// ParticleAnimatedTrait
// ===========================================================================


bool ParticleAnimatedTrait::UpdateParticle(float delta_time, ParticleAnimated* particle)
{
	if (particle->bitmap_info != nullptr && particle->bitmap_info->HasAnimation())
	{
		float frame_time = particle->bitmap_info->GetFrameTime();
		if (frame_time <= 0.0f)
			frame_time = 1 / 5.0f;

		particle->animation_timer += delta_time / frame_time;

		particle->frame_index = (int)particle->animation_timer;
	}

	return false;
}

void ParticleAnimatedTrait::ParticleToPrimitives(ParticleAnimated const& particle, chaos::QuadOutput<VertexBase>& output)
{
	chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();

	chaos::ParticleTexcoords texcoords = DoGetAnimatedParticleTexcoords(particle, chaos::WrapMode::wrap);

	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, texcoords, primitive);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < primitive.count; ++i)
		primitive[i].color = particle.color;
}


// ===========================================================================
// ParticleBloodTrait
// ===========================================================================

static bool DoUpdateBloodParticle(float delta_time, ParticleAnimated* particle, float animation_rate)
{
	particle->bounding_box.position += delta_time * particle->velocity;

	if (particle->duration > 0.0f)
	{
		particle->life += delta_time;
		if (particle->life > particle->duration)
			return true;

		particle->color.a = 1.0f - (particle->life / particle->duration);
	}

	if (particle->bitmap_info != nullptr && particle->bitmap_info->HasAnimation())
	{
		int anim_count = (int)particle->bitmap_info->GetAnimationImageCount();

		particle->animation_timer += animation_rate * delta_time * (particle->life / particle->duration); // count on CLAMP to avoid loop

		// shu46 : manual clamp
		particle->frame_index = std::min(
			anim_count - 1,
			(int)((float)anim_count * particle->animation_timer));
	}
	return false;
}

// ===========================================================================
// ParticleBloodTrait
// ===========================================================================

void ParticleBloodTrait::ParticleToPrimitives(ParticleBlood const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
	chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();

	chaos::box2 box = particle.bounding_box;

	box.half_size *= 1.0f + (particle.life / particle.duration);

	chaos::ParticleTexcoords texcoords = DoGetAnimatedParticleTexcoords(particle, chaos::WrapMode::clamp);

	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(box, texcoords, primitive);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < primitive.count; ++i)
		primitive[i].color = particle.color;
}

bool ParticleBloodTrait::UpdateParticle(float delta_time, ParticleBlood* particle, LayerTrait const* layer_trait) const
{
	if (DoUpdateBloodParticle(delta_time, particle, 1.0f))
		return true;
	particle->velocity += delta_time * particle->acceleration;
	return false;
}


// ===========================================================================
// ParticleBurnedSoulTrait
// ===========================================================================

void ParticleBurnedSoulTrait::ParticleToPrimitives(ParticleBurnedSoul const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
	chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();

	chaos::box2 box = particle.bounding_box;
	box.position.x += 50.0f * std::sin(particle.offset_t);

		
	chaos::ParticleTexcoords texcoords = DoGetAnimatedParticleTexcoords(particle, chaos::WrapMode::clamp);
	
	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(box, texcoords, primitive);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < primitive.count; ++i)
		primitive[i].color = particle.color;
}

bool ParticleBurnedSoulTrait::UpdateParticle(float delta_time, ParticleBurnedSoul* particle, LayerTrait const* layer_trait) const
{
	if (DoUpdateBloodParticle(delta_time, particle, 2.0f))
		return true;
	particle->velocity += delta_time * particle->acceleration;
	particle->offset_t += delta_time;

	return false;
}


// ===========================================================================
// ParticlePlayerTrait
// ===========================================================================


// ===========================================================================
// ParticlePlayerTrait
// ===========================================================================

void ParticlePlayerTrait::ParticleToPrimitives(ParticlePlayer const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
   // 

	if (particle.bitmap_info != nullptr && particle.bitmap_info->HasAnimation())
	{
		chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();

		chaos::BitmapAtlas::BitmapLayout layout = particle.bitmap_info->GetAnimationLayout(particle.frame_index, chaos::WrapMode::clamp);

		chaos::ParticleTexcoords texcoords = chaos::ParticleTools::GetParticleTexcoords(layout); // shu46 : should be a member of BitmapLayout itself shouldnt it??!

		texcoords = chaos::ParticleTools::ApplySymetriesToTexcoords(texcoords, particle.horizontal_flip, particle.vertical_flip, particle.diagonal_flip);

		// generate particle corners and texcoords
		chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, texcoords, primitive);
		// copy the color in all triangles vertex
		for (size_t i = 0; i < primitive.count; ++i)
			primitive[i].color = particle.color;

	}
	else
	{
		chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives(particle, output);
	}
}

bool ParticlePlayerTrait::UpdateParticle(float delta_time, ParticlePlayer* particle, LayerTrait const* layer_trait) const
{
	LudumPlayerDisplacementComponent* displacement_component = layer_trait->game->GetPlayerDisplacementComponent(0);
	if (displacement_component != nullptr)
	{
		glm::vec2 pawn_velocity = displacement_component->GetPawnVelocity();

		if (std::abs(pawn_velocity.x) < 1.0f)
		{
			particle->frame_index = 0;
			particle->horizontal_flip = false;	
		}
		else if (std::abs(pawn_velocity.x) < 12.0f)
		{
			particle->frame_index = 3;
			particle->horizontal_flip = (pawn_velocity.x < 0.0f);
		}
		else 
		{
			float max_pawn_velocity = displacement_component->GetDisplacementInfo().max_pawn_velocity.x;

			float speed_factor = 8.0f;
			if (max_pawn_velocity > 0.0f)
				speed_factor = std::max(1.0f, speed_factor * std::abs(pawn_velocity.x) / max_pawn_velocity);

			particle->animation_timer += speed_factor * delta_time;

			particle->horizontal_flip = (pawn_velocity.x < 0.0f);
			particle->frame_index = 1 + (int)std::fmodf(particle->animation_timer, 2.0f);


		}

	}


	return false;
}



