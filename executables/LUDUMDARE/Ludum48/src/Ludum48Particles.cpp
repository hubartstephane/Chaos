#pragma once
#include "Ludum48PCH.h"
#include "Ludum48Particles.h"
#include "Ludum48Game.h"
#include "Ludum48GameInstance.h"
#include "Ludum48Player.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48Level.h"
#include "Ludum48PlayerDisplacementComponent.h"

// ===========================================================================
// ParticleGateLayerTrait
// ===========================================================================


void ParticleSmokeLayerTrait::ParticleToPrimitives(SmokeParticle const& particle, PrimitiveOutput<VertexBase>& output) const
{
	SmokeParticle other = particle;

	other.color.a = 1.0f - (particle.age / particle.lifetime);

	float box_ratio = 1.0f + 0.4f * (particle.age / particle.lifetime);

	other.bounding_box.half_size = particle.bounding_box.half_size * box_ratio;

	return ::ParticleToPrimitives(other, output);
}

bool ParticleSmokeLayerTrait::UpdateParticle(float delta_time, SmokeParticle& particle) const
{
	particle.age += delta_time;
	if (particle.age > particle.lifetime)
		return true;

	particle.bounding_box.position += particle.velocity * delta_time;



	return false;
}





// ===========================================================================
// ParticleGateLayerTrait
// ===========================================================================


void ParticleGateLayerTrait::ParticleToPrimitives(GateParticle const& particle, PrimitiveOutput<VertexBase>& output) const
{
	bool door_opened = false;

	GateParticle other = particle;


	LudumLevelInstance* li = game->GetLevelInstance();
	if (li != nullptr)
		door_opened = li->door_opened;

	WindowApplication const * application = Application::GetConstInstance();
	if (application != nullptr)
	{
		BitmapAtlas::TextureArrayAtlas const * atlas = application->GetTextureAtlas();
		if (atlas != nullptr)
		{
			BitmapAtlas::BitmapInfo const* bi = (door_opened) ?
				atlas->GetBitmapInfo("OpenGate", true) :
				atlas->GetBitmapInfo("Gate", true);

			if (bi != nullptr)
				other.texcoords = bi->GetTexcoords();
		}
	}

	return ::ParticleToPrimitives(other, output);
}

bool ParticleGateLayerTrait::UpdateParticle(float delta_time, GateParticle& particle) const
{





	return false;
}


// ===========================================================================
// ParticleGameObjectLayerTrait
// ===========================================================================

void ParticleGameObjectLayerTrait::ParticleToPrimitives(GameObjectParticle const& particle, PrimitiveOutput<VertexBase>& output) const
{
	GameObjectParticle copy = particle;
	copy.bounding_box.position += particle.offset * tile_size;
	::ParticleToPrimitives(copy, output);
}

bool ParticleGameObjectLayerTrait::UpdateParticle(float delta_time, GameObjectParticle& particle) const
{
	if (particle.destroy_particle)
		return true;


	if (particle.type == GameObjectType::Rock)
	{
		if (particle.direction.x != 0)
		{
			float constexpr ROTATION_SPEED = 10.0f;

			float speed_factor = 1.0f;
			particle.rotation -= ROTATION_SPEED * delta_time * particle.direction.x * speed_factor;
		}
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
		BitmapAtlas::BitmapLayout layout = particle.bitmap_info->GetAnimationLayoutFromTime(particle.animation_timer);
		if (!layout.IsValid())
			return false;
		particle.texcoords = layout.GetTexcoords();
	}
	return true;
}





// ===========================================================================
// ParticleAnimatedLayerTrait
// ===========================================================================


bool ParticleAnimatedLayerTrait::UpdateParticle(float delta_time, ParticleAnimated & particle) const
{
	if (particle.destroy_particle)
		return true;

	particle.animation_timer += delta_time;

	// destroy the particles ?
	if (!UpdateAnimatedParticleTexcoords(particle))
		return true;

	return false;
}

void ParticleAnimatedLayerTrait::ParticleToPrimitives(ParticleAnimated const& particle, PrimitiveOutput<VertexBase>& output) const
{
	ParticleAnimated copy = particle;
	copy.bounding_box.position += particle.offset * glm::vec2(32.0f, 32.0f); // HACK



	int reversed_flag = (copy.flags & ParticleFlags::TEXTURE_HORIZONTAL_FLIP);

	if (copy.direction.x != 0.0f || copy.direction.y != 0.0f)
	{
		if (copy.direction.y != 0.0f)
		{
			copy.flags &= ~ParticleFlags::TEXTURE_HORIZONTAL_FLIP;
			copy.flags |= reversed_flag;
		}
		else if (particle.direction.x > 0.0f)
			copy.flags &= ~ParticleFlags::TEXTURE_HORIZONTAL_FLIP;
		else
			copy.flags |= ParticleFlags::TEXTURE_HORIZONTAL_FLIP;

	}


	::ParticleToPrimitives(copy, output);
}

// ===========================================================================
// ParticlePlayerLayerTrait
// ===========================================================================

void ParticlePlayerLayerTrait::ParticleToPrimitives(ParticlePlayer const& particle, PrimitiveOutput<VertexBase>& output) const
{
	ParticlePlayer copy = particle;
	copy.bounding_box.position += particle.offset * tile_size;
	::ParticleToPrimitives(copy, output);
}


bool ParticlePlayerLayerTrait::UpdateParticle(float delta_time, ParticlePlayer & particle) const
{
	static constexpr float SPEED_FACTOR = 2.0f;

	int reversed_flag = (particle.flags & ParticleFlags::TEXTURE_HORIZONTAL_FLIP);

	float & idle_timer = particle.idle_timer;

	if (particle.direction.x != 0.0f || particle.direction.y != 0.0f)
	{
		idle_timer = 0.0f;

		particle.animation_timer += SPEED_FACTOR * delta_time * particle.speed;
		particle.frame_index = 1 + (int)std::fmodf(particle.animation_timer, 2.0f);

		if (particle.direction.y != 0.0f)
		{
			particle.flags &= ~ParticleFlags::TEXTURE_HORIZONTAL_FLIP;
			particle.flags |= reversed_flag;
		}
		else if (particle.direction.x > 0.0f)
			particle.flags &= ~ParticleFlags::TEXTURE_HORIZONTAL_FLIP;
		else
			particle.flags |= ParticleFlags::TEXTURE_HORIZONTAL_FLIP;

	}
	else
	{
		idle_timer = std::min(idle_timer + delta_time, 1.0f);
		if (idle_timer == 1.0f)
			particle.frame_index = 0;

	}

	if (particle.bitmap_info != nullptr && particle.bitmap_info->HasAnimation())
	{
		BitmapAtlas::BitmapLayout layout = particle.bitmap_info->GetAnimationLayout(particle.frame_index);
		if (!layout.IsValid())
			return true; // destroy the particle

		particle.texcoords = layout.GetTexcoords();
	}
	return particle.destroy_particle;
}
