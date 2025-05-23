#pragma once

#include "Ludum44PCH.h"
#include "Ludum44Particles.h"
#include "Ludum44Game.h"
#include "Ludum44GameInstance.h"
#include "Ludum44LevelInstance.h"

// ===========================================================================
// FindEnemiesOnMap
// ===========================================================================


static bool ObjectBesideCamera(chaos::box2 const & camera_box, chaos::box2 const & object_box)
{
	float cam_y = camera_box.position.y - camera_box.half_size.y;
	float obj_y = object_box.position.y + object_box.half_size.y;
	if (obj_y < cam_y)
		return true;
	return false;

}


static void FindEnemiesOnMap(LudumGame * game, std::vector<ParticleEnemy*> & result)
{
	// get the enemies
	LudumLevelInstance* ludum_level_instance = game->GetLevelInstance();

	chaos::TMLayerInstance * enemies_layer_instance = ludum_level_instance->FindLayerInstance("Enemies", true);
	if (enemies_layer_instance != nullptr)
	{
		chaos::ParticleLayerBase * layer = enemies_layer_instance->GetParticleLayer();
		if (layer != nullptr)
		{
			size_t allocation_count = layer->GetAllocationCount();
			for (size_t i = 0 ; i < allocation_count ; ++i)
			{
				chaos::ParticleAllocationBase * allocation = layer->GetAllocation(i);
				if (allocation != nullptr)
				{
					chaos::ParticleAccessor<ParticleEnemy> enemies = allocation->GetParticleAccessor();
					size_t count = enemies.GetDataCount();
					for (size_t j = 0 ; j < count ; ++j)
						result.push_back(&enemies[j]);
				}
			}
		}
	}
}

static float OnCollisionWithEnemy(ParticleEnemy * enemy, float damage, LudumGame * game, bool collision_with_player, chaos::box2 const & ref_box) // returns the life damage produced by the enemy collision (its life)
{
	float result = collision_with_player? enemy->damage_for_player : enemy->life;

	// update life from both size
	enemy->life -= damage;
	enemy->touched_count_down = 2;

	// play sound
	if (enemy->life > 0.0f)
		game->PlaySound("metallic", false, false, 0.0f, chaos::SoundContext::LEVEL);
	else
	{
		if (!collision_with_player)
			game->GetPlayer(0)->SetScore(enemy->score, true);
		game->PlaySound("explosion", false, false, 0.0f, chaos::SoundContext::LEVEL);

		LudumGameInstance* ludum_game_instance = game->GetGameInstance();
		ludum_game_instance->FireExplosion(ref_box);
	}
	return result;
}

// ===========================================================================
// ParticlePlayerLayerTrait
// ===========================================================================

bool ParticlePlayerLayerTrait::UpdateParticle(float delta_time, ParticlePlayer& particle) const
{
	// find all enemies
	std::vector<ParticleEnemy*> enemies;
	FindEnemiesOnMap(game, enemies);

	// seach a collision for all enemies
	size_t count = enemies.size();
	for (size_t i = 0 ; i < count ; ++i)
	{
		ParticleEnemy * enemy = enemies[i];
		if (enemy->life > 0.0f)
		{
			if (chaos::Collide(particle.bounding_box, enemy->bounding_box))
			{
				float life_lost = OnCollisionWithEnemy(enemy, enemy->life, game, true, enemy->bounding_box); // destroy the enemy always

				LudumPlayer * player = game->GetPlayer(0);
				player->SetHealth(-life_lost, true);
			}
		}
	}

	// displace the player
	particle.bounding_box.position += delta_time * particle.velocity;

	return false;
}



// =====================================
// PowerUpZoneParticleLayerTrait
// =====================================

void GetTypedVertexDeclaration(chaos::GPUVertexDeclaration * result, boost::mpl::identity<VertexPowerUpZone>)
{
	GetTypedVertexDeclaration(result, boost::mpl::identity<VertexBase>());

    result->Push(chaos::VertexAttributeSemantic::TEXCOORD, 1, chaos::VertexAttributeType::FLOAT3); // top-right of sprite in atlas (bottom-left was in parent function)
    result->Push(chaos::VertexAttributeSemantic::TEXCOORD, 2, chaos::VertexAttributeType::FLOAT2);
}

bool PowerUpZoneParticleLayerTrait::UpdateParticle(float delta_time, ParticlePowerUpZone& particle)
{
	// XXX: see UpdatePlayerBuyingItem(...)	=> particle.gid = 0;
	//          this was usefull to require a zone destruction
	//          this is not used anymore
	if (particle.gid == 0)
	{
		particle.color.a -= delta_time;
		if (particle.color.a <= 0.0f) // fade out the particle
			return true;
	}
	return false;
}


void PowerUpZoneParticleLayerTrait::ParticleToPrimitives(chaos::TMParticle const& particle, chaos::GPUPrimitiveOutput<VertexPowerUpZone>& output) const
{
    chaos::QuadPrimitive<VertexPowerUpZone> primitive = output.AddQuads();

    chaos::ParticleToPrimitive(particle, primitive);

    VertexPowerUpZone& v0 = primitive[0];
    VertexPowerUpZone& v1 = primitive[1];
    VertexPowerUpZone& v2 = primitive[2];
    VertexPowerUpZone& v3 = primitive[3];

    glm::vec3 texture_bl = v0.texcoord;
    glm::vec3 texture_tr = v2.texcoord;

    glm::vec2 position_bl = v0.position;
    glm::vec2 position_tr = v2.position;

    // override the texture coordinates
    for (VertexPowerUpZone & vertex : primitive)
    {
        vertex.texcoord = texture_bl;
        vertex.texcoord2 = texture_tr;
    }

    // compute repetition
    glm::vec2 repetition = glm::vec2(1.0f, 1.0f);

    v0.texcoord3 = repetition * glm::vec2(0.0f, 0.0f);
    v1.texcoord3 = repetition * glm::vec2(1.0f, 0.0f);
    v2.texcoord3 = repetition * glm::vec2(1.0f, 1.0f);
    v3.texcoord3 = repetition * glm::vec2(0.0f, 1.0f);
}

// ===========================================================================
// ParticleExplosionLayerTrait
// ===========================================================================


bool ParticleExplosionLayerTrait::UpdateParticle(float delta_time, ParticleExplosion& particle) const
{
	if (particle.explosion_info == nullptr) // delete the particle
		return true;

	size_t image_count = particle.explosion_info->GetAnimationImageCount();
	float frame_duration = particle.explosion_info->GetFrameDuration();

	int image_index = (int)(particle.age / frame_duration);

	chaos::AtlasBitmapLayout bitmap_layout = particle.explosion_info->GetAnimationLayout(image_index, chaos::WrapMode::CHECK_RANGES);
	if (bitmap_layout.bitmap_index < 0)
		return true;

	particle.age += delta_time;

#if 0
	particle.texcoords = bitmap_layout;

	// compute the bounding box for all particles
	chaos::box2 particle_box = ref_box;

	particle_box.half_size = ratio_to_box * ref_box.half_size;

	particle_box.position = ref_box.position;

	// compute texcoords for all particles
#endif
	particle.texcoords = bitmap_layout.GetTexcoords();



	return false;
}

// ===========================================================================
// ParticleLifeLayerTrait
// ===========================================================================


bool ParticleLifeLayerTrait::UpdateParticle(float delta_time, ParticleLife& particle) const
{

	return false;
}

// ===========================================================================
// ParticleFireLayerTrait
// ===========================================================================


ParticleFireUpdateData ParticleFireLayerTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleFire> & particle_accessor) const
{
	ParticleFireUpdateData result;
	if (particle_accessor.GetDataCount() > 0)
	{
		// get the camera box
		chaos::Camera const* camera = game->GetCamera(0);
		if (camera != nullptr)
			result.camera_box = camera->GetCameraBox(true);
		//result.camera_box.half_size *= 3.0f;
		// get the enemies
		FindEnemiesOnMap(game, result.enemies);
		// get the players
		result.player = game->GetPlayer(0);
	}
	return result;
}


bool ParticleFireLayerTrait::UpdateParticle(float delta_time, ParticleFire& particle, ParticleFireUpdateData const & update_data) const
{

	// all damage consummed
	if (particle.damage <= 0.0f)
		return true;
	// outside the camera

	particle.lifetime -= delta_time;
	if (particle.lifetime <= 0.0f)
		return true;

	if (particle.player_ownership && !chaos::Collide(update_data.camera_box, particle.bounding_box)) // destroy the particle outside the camera frustum (works for empty camera)
		return true;

	if (!particle.player_ownership && ObjectBesideCamera(update_data.camera_box, particle.bounding_box))
		return true;

	chaos::PlayerPawn* player_pawn = update_data.player->GetPawn();
	if (player_pawn == nullptr)
		return false;


	// search for collisions
	if (particle.player_ownership)
	{
		size_t count = update_data.enemies.size();
		for (size_t i = 0 ; i < count ; ++i)
		{
			ParticleEnemy * enemy = update_data.enemies[i];
			if (enemy->life > 0.0f)
			{
				if (chaos::Collide(particle.bounding_box, enemy->bounding_box))
				{
					particle.damage -= OnCollisionWithEnemy(enemy, particle.damage, game, false, enemy->bounding_box);

					// kill bullet ?
					if (particle.damage <= 0.0f)
						return true;
					if (!particle.trample)
						return true;
				}
			}
		}
	}
	// enemy bullet



	chaos::box2 pawn_box = player_pawn->GetBoundingBox();
	pawn_box.half_size *= 0.7f;
	if (!particle.player_ownership && update_data.player != nullptr)
	{
		if (chaos::Collide(particle.bounding_box, pawn_box)) // destroy the particle outside the camera frustum (works for empty camera)
		{
			update_data.player->SetHealth(-particle.damage, true);
			particle.damage = 0.0f;

			game->PlaySound("player_touched", false, false, 0.0f, chaos::SoundContext::LEVEL);
		}
	}

	// update position velocity
	particle.bounding_box.position += delta_time * particle.velocity;

	return false; // do not destroy the particle
}

void ParticleFireLayerTrait::ParticleToPrimitives(ParticleFire const& particle, chaos::GPUPrimitiveOutput<VertexBase>& output) const
{
	ParticleFire other = particle;
	other.color.a = (other.lifetime < 1.0f) ? other.lifetime : 1.0f;

	chaos::ParticleToPrimitives(other, output);
}

// ===========================================================================
// ParticleEnemyLayerTrait
// ===========================================================================

ParticleEnemyUpdateData ParticleEnemyLayerTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleEnemy> & particle_accessor) const
{
	ParticleEnemyUpdateData result;
	if (particle_accessor.GetDataCount() > 0)
	{
		chaos::Camera const * camera = game->GetCamera(0);
		if (camera != nullptr)
			result.camera_box = camera->GetCameraBox(true);
		//result.camera_box.half_size *= 3.0f;
	}
	return result;
}

bool ParticleEnemyLayerTrait::UpdateParticle(float delta_time, ParticleEnemy& particle, ParticleEnemyUpdateData const & update_data) const
{
	// destroy the particle if no life
	if (particle.life <= 0.0f)
		return true;
	// destroy the particle if outside a BIG camera box

	// shuxxx
	//if (!chaos::Collide(update_data.camera_box, particle.bounding_box)) // destroy the particle outside the camera frustum (works for empty camera)
	//	return true;

	if (ObjectBesideCamera(update_data.camera_box, particle.bounding_box))
		return true;

	// apply velocity
	particle.bounding_box.position += delta_time * particle.velocity;
	// apply rotation

	if (particle.rotation_following_player)
	{
		chaos::PlayerPawn * player_pawn = game->GetPlayerPawn(0);
		if (player_pawn != nullptr)
		{
			glm::vec2 delta_pos = player_pawn->GetPosition() - particle.bounding_box.position;
			particle.rotation = atan2f(delta_pos.y, delta_pos.x) - (float)M_PI_2;
		}
	}
	else
	{
		particle.rotation += delta_time * particle.rotation_speed;
	}

	// update blinking effect
	if (particle.touched_count_down > 0)
		--particle.touched_count_down;



	if (particle.fire_frequency > 0.0f)
	{
		particle.current_fire_timer += delta_time;
		if (particle.current_fire_timer >= particle.fire_frequency)
		{
			float size_ratio = 0.2f;

            int count = 4;

			float delta_angle = 2.0f * (float)M_PI / (float)count;

			LudumGameInstance* ludum_game_instance = game->GetGameInstance();
			ludum_game_instance->FireProjectile("enemy_fire", particle.bounding_box, size_ratio, count, nullptr, particle.rotation, delta_angle, game->enemy_fire_velocity, game->enemy_fire_damage, false, false);

			particle.current_fire_timer = 0.0f;
		}
	}

	return false; // do not destroy the particle
}

void ParticleEnemyLayerTrait::ParticleToPrimitives(ParticleEnemy const& particle, chaos::GPUPrimitiveOutput<VertexBase>& output) const
{
	ParticleEnemy other = particle;
	other.color.a = (other.touched_count_down > 0) ? 0.0f : 1.0f;

	chaos::ParticleToPrimitives(other, output);
}
