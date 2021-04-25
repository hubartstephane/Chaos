#pragma once

#include <chaos/Chaos.h>

#include "Ludum48Particles.h"
#include "Ludum48Game.h"
#include "Ludum48GameInstance.h"
#include "Ludum48Player.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48Level.h"
#include "Ludum48PlayerDisplacementComponent.h"

// ===========================================================================
// ParticleGameObjectLayerTrait
// ===========================================================================

void ParticleGameObjectLayerTrait::ParticleToPrimitives(GameObjectParticle const& particle, chaos::PrimitiveOutput<VertexBase>& output) const
{
	if (particle.type == GameObjectType::Diamond)
	{




	}



	LudumGameInstance const* ludum_game_instance = game->GetGameInstance();

	chaos::ParticleDefault copy = particle;
	copy.bounding_box.position += particle.offset * tile_size;
	chaos::ParticleToPrimitives(copy, output);
}

bool ParticleGameObjectLayerTrait::UpdateParticle(float delta_time, GameObjectParticle& particle) const
{
	if (particle.type == GameObjectType::Rock)
	{
		if (particle.direction.x != 0)
		{
			float constexpr ROTATION_SPEED = 10.0f;

			float speed_factor = 1.0f;
			LudumLevelInstance const* li = game->GetLevelInstance();
			if (li != nullptr)
				speed_factor = li->speed_factor;
			particle.rotation -= ROTATION_SPEED * delta_time * particle.direction.x * speed_factor;
		}
	}






	if (particle.destroy_particle && particle.locked_cell != nullptr)
		particle.locked_cell->UnLock(&particle);
	return particle.destroy_particle;
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

// ===========================================================================
// ParticlePlayerLayerTrait
// ===========================================================================

void ParticlePlayerLayerTrait::ParticleToPrimitives(ParticlePlayer const& particle, chaos::PrimitiveOutput<VertexBase>& output) const
{




	LudumGameInstance const* ludum_game_instance = game->GetGameInstance();

	chaos::ParticleDefault copy = particle;
	copy.bounding_box.position += particle.offset * tile_size;
	chaos::ParticleToPrimitives(copy, output);
}


bool ParticlePlayerLayerTrait::UpdateParticle(float delta_time, ParticlePlayer & particle) const
{
	static constexpr float SPEED_FACTOR = 2.0f;

	int reversed_flag = (particle.flags & chaos::ParticleFlags::TEXTURE_HORIZONTAL_FLIP);

	if (particle.direction.x != 0.0f || particle.direction.y != 0.0f)
	{
		particle.animation_timer += SPEED_FACTOR * delta_time * particle.speed;
		particle.frame_index = 1 + (int)std::fmodf(particle.animation_timer, 2.0f);

		if (particle.direction.y != 0.0f)
		{
			particle.flags &= ~chaos::ParticleFlags::TEXTURE_HORIZONTAL_FLIP;
			particle.flags |= reversed_flag;
		}
		else if (particle.direction.x > 0.0f)
			particle.flags &= ~chaos::ParticleFlags::TEXTURE_HORIZONTAL_FLIP;
		else
			particle.flags |= chaos::ParticleFlags::TEXTURE_HORIZONTAL_FLIP;

	}
	else 
	{
		particle.frame_index = 0;

	}

	if (particle.bitmap_info != nullptr && particle.bitmap_info->HasAnimation())
	{
		chaos::BitmapAtlas::BitmapLayout layout = particle.bitmap_info->GetAnimationLayout(particle.frame_index);
		if (!layout.IsValid())
			return true; // destroy the particle

		particle.texcoords = layout.GetTexcoords();
	}


	if (particle.destroy_particle && particle.locked_cell != nullptr)
		particle.locked_cell->UnLock(&particle);
	return particle.destroy_particle;
}

// ===========================================================================
// Standalone function
// ===========================================================================


bool UpdateParticlePositionInGrid(GameObjectParticle* particle, float delta_time, class GridInfo& grid_info)
{
	bool result = (particle->direction.y < 0.0f);
	for (int axis : {0, 1})
	{
		if (particle->direction[axis] != 0.0f)
		{
			particle->offset[axis] = std::clamp(particle->offset[axis] + particle->speed * delta_time * particle->direction[axis], -1.0f, 1.0f);
			if (particle->offset[axis] == -1.0f || particle->offset[axis] == 1.0f)
			{
				if (grid_info(particle->bounding_box.position).particle == particle)
					grid_info(particle->bounding_box.position).particle = nullptr;
				particle->bounding_box.position += particle->direction * chaos::RecastVector<glm::vec2>(grid_info.tile_size);
				grid_info(particle->bounding_box.position).particle = particle;
				grid_info(particle->bounding_box.position).UnLock(particle);

				particle->offset = { 0.0f, 0.0f };
				particle->direction = { 0.0f, 0.0f };

				return result; // object was falling and is now stopped
			}
		}
	}
	return false; // movement not finished yet
}


