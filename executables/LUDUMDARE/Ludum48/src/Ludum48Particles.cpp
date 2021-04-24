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
// ParticlePlayerLayerTrait
// ===========================================================================

bool ParticlePlayerLayerTrait::UpdateParticle(float delta_time, ParticlePlayer & particle) const
{
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






		if (displacement_component->displacement_state == chaos::PlatformerDisplacementState::GROUNDED)
			particle.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		else
			particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);



	}

	if (particle.bitmap_info != nullptr && particle.bitmap_info->HasAnimation())
	{
		chaos::BitmapAtlas::BitmapLayout layout = particle.bitmap_info->GetAnimationLayout(particle.frame_index);
		if (!layout.IsValid())
			return true; // destroy the particle

		particle.texcoords = layout.GetTexcoords();
	}






	return false;
}



