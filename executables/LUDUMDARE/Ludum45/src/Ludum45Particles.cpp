#pragma once


#include "Ludum45Particles.h"
#include "Ludum45Game.h"
#include "Ludum45GameInstance.h"
#include "Ludum45LevelInstance.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>
#include <chaos/ParticleTools.h>

#include <death/SoundContext.h>

chaos::GPUVertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<VertexBase>)
{
	chaos::GPUVertexDeclaration result;
	result.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT2);
	result.Push(chaos::SEMANTIC_TEXCOORD, 0, chaos::TYPE_FLOAT3);
	result.Push(chaos::SEMANTIC_COLOR, 0, chaos::TYPE_FLOAT4);
	return result;
}


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
	death::TiledMap::LayerInstance * enemies_layer_instance = game->GetLudumLevelInstance()->FindLayerInstance("Enemies");
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
					chaos::ParticleAccessor<ParticleEnemy> enemies = allocation->GetParticleAccessor<ParticleEnemy>();
					size_t count = enemies.GetCount();
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
		game->Play("metallic", false, false, 0.0f, death::SoundContext::LEVEL);
	else 
	{
		if (!collision_with_player)
			game->GetPlayer(0)->SetScore(enemy->score, true);
		game->Play("explosion", false, false, 0.0f, death::SoundContext::LEVEL);
		game->GetLudumGameInstance()->FireExplosion(ref_box);
	}
	return result;
}

// ===========================================================================
// ParticlePlayerTrait
// ===========================================================================

size_t ParticlePlayerTrait::ParticleToVertices(ParticlePlayer const * p, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const
{
	return chaos::ParticleDefault::ParticleTrait::ParticleToVertices(p, vertices, vertices_per_particle);
}


bool ParticlePlayerTrait::UpdateParticle(float delta_time, ParticlePlayer * particle, LayerTrait const * layer_trait) const
{
	// find all enemies
	std::vector<ParticleEnemy*> enemies;
	FindEnemiesOnMap(layer_trait->game, enemies);

	// seach a collision for all enemies
	size_t count = enemies.size();
	for (size_t i = 0 ; i < count ; ++i)
	{
		ParticleEnemy * enemy = enemies[i];
		if (enemy->life > 0.0f)
		{
			if (chaos::Collide(particle->bounding_box, enemy->bounding_box))
			{
				float life_lost = OnCollisionWithEnemy(enemy, enemy->life, layer_trait->game, true, enemy->bounding_box); // destroy the enemy always
			
				LudumPlayer * player = layer_trait->game->GetLudumPlayer(0);
				player->SetLifeBarValue(-life_lost, true);
			}
		}
	}

	// displace the player
	particle->bounding_box.position += delta_time * particle->velocity;

	return false;
}



// =====================================
// PowerUpZoneParticleTrait
// =====================================

chaos::GPUVertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<VertexPowerUpZone>)
{
	chaos::GPUVertexDeclaration result;
	result.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT2);
	result.Push(chaos::SEMANTIC_TEXCOORD, 0, chaos::TYPE_FLOAT3); // bottom-left of sprite in atlas
	result.Push(chaos::SEMANTIC_COLOR, 0, chaos::TYPE_FLOAT4);	
	result.Push(chaos::SEMANTIC_TEXCOORD, 1, chaos::TYPE_FLOAT3); // top-right of sprite in atlas
	result.Push(chaos::SEMANTIC_TEXCOORD, 2, chaos::TYPE_FLOAT2);
	return result;
}

bool PowerUpZoneParticleTrait::UpdateParticle(float delta_time, ParticlePowerUpZone * particle)
{
	// XXX: see UpdatePlayerBuyingItem(...)	=> particle.gid = 0;
	//          this was usefull to require a zone destruction
	//          this is not used anymore
	if (particle->gid == 0)
	{
		particle->color.a -= delta_time;
		if (particle->color.a <= 0.0f) // fade out the particle
			return true;	
	}
	return false;
}
size_t PowerUpZoneParticleTrait::ParticleToVertices(death::TiledMap::TileParticle const * particle, VertexPowerUpZone * vertices, size_t vertices_per_particle)
{
	size_t result = chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, vertices, vertices_per_particle);

	// get the texture coordinates in the atlas
	glm::vec3 texture_bl = vertices[0].texcoord;
	glm::vec3 texture_tr = vertices[2].texcoord;

	glm::vec2 position_bl = vertices[0].position;
	glm::vec2 position_tr = vertices[2].position;

	// override the texture coordinates
	for (size_t i = 0; i < 6; ++i)
	{
		vertices[i].texcoord  = texture_bl;
		vertices[i].texcoord2 = texture_tr;
	}

	// compute repetition
	glm::vec2 repetition = glm::vec2(1.0f, 1.0f);

	vertices[0].texcoord3 = vertices[3].texcoord3 = repetition * glm::vec2(0.0f, 0.0f);
	vertices[1].texcoord3 = repetition * glm::vec2(1.0f, 0.0f);
	vertices[2].texcoord3 = vertices[4].texcoord3 = repetition * glm::vec2(1.0f, 1.0f);
	vertices[5].texcoord3 = repetition * glm::vec2(0.0f, 1.0f);

	return result;
}



// ===========================================================================
// ParticleExplosionTrait
// ===========================================================================


bool ParticleExplosionTrait::UpdateParticle(float delta_time, ParticleExplosion * particle, LayerTrait const * layer_trait) const
{
	if (particle->explosion_info == nullptr) // delete the particle
		return true;

	int image_count = particle->explosion_info->GetAnimationImageCount();
	float frame_time = (float)particle->explosion_info->GetFrameTime();

	if (frame_time == 0)
		frame_time = 1 / 16.0f;

	int image_index = (int)(particle->age / frame_time);

	chaos::BitmapAtlas::BitmapLayout bitmap_layout = particle->explosion_info->GetAnimationLayout(image_index, chaos::BitmapAtlas::GetBitmapLayoutFlag::none);
	if (bitmap_layout.bitmap_index < 0)
		return true;

	particle->age += delta_time;

#if 0
	particle->texcoords = bitmap_layout;

	// compute the bounding box for all particles
	chaos::box2 particle_box = ref_box;

	particle_box.half_size = ratio_to_box * ref_box.half_size;
	
	particle_box.position = ref_box.position;

	// compute texcoords for all particles
#endif
	particle->texcoords = chaos::ParticleTools::GetParticleTexcoords(bitmap_layout, layer_trait->game->GetTextureAtlas()->GetAtlasDimension());
	


	return false;
}

size_t ParticleExplosionTrait::ParticleToVertices(ParticleExplosion const * particle, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const
{


	return chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, vertices, vertices_per_particle);
}




// ===========================================================================
// ParticleLifeTrait
// ===========================================================================


bool ParticleLifeTrait::UpdateParticle(float delta_time, ParticleLife * particle, LayerTrait const * layer_trait) const
{

	return false;
}

size_t ParticleLifeTrait::ParticleToVertices(ParticleLife const * particle, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const
{
	return chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, vertices, vertices_per_particle);
}


// ===========================================================================
// ParticleFireTrait
// ===========================================================================


ParticleFireUpdateData ParticleFireTrait::BeginUpdateParticles(float delta_time, ParticleFire * particle, size_t count, LayerTrait const * layer_trait) const
{
	ParticleFireUpdateData result;
	if (count > 0)
	{
		// get the camera box 
		result.camera_box = layer_trait->game->GetLudumLevelInstance()->GetCameraBox(0);
		//result.camera_box.half_size *= 3.0f;
		// get the enemies
		FindEnemiesOnMap(layer_trait->game, result.enemies);
		// get the players
		result.player = layer_trait->game->GetLudumPlayer(0);
	}
	return result;
}


bool ParticleFireTrait::UpdateParticle(float delta_time, ParticleFire * particle, ParticleFireUpdateData const & update_data, LayerTrait const * layer_trait) const
{
	// all damage consummed
	if (particle->damage <= 0.0f)
		return true;
	// outside the camera

	particle->lifetime -= delta_time;
	if (particle->lifetime <= 0.0f)
		return true;

	if (particle->player_ownership && !chaos::Collide(update_data.camera_box, particle->bounding_box)) // destroy the particle outside the camera frustum (works for empty camera)
		return true;	

	if (!particle->player_ownership && ObjectBesideCamera(update_data.camera_box, particle->bounding_box))
		return true;

	// search for collisions
	if (particle->player_ownership)
	{
		size_t count = update_data.enemies.size();
		for (size_t i = 0 ; i < count ; ++i)
		{
			ParticleEnemy * enemy = update_data.enemies[i];
			if (enemy->life > 0.0f)
			{
				if (chaos::Collide(particle->bounding_box, enemy->bounding_box))
				{
					particle->damage -= OnCollisionWithEnemy(enemy, particle->damage, layer_trait->game, false, enemy->bounding_box);

					// kill bullet ?
					if (particle->damage <= 0.0f)
						return true;
					if (!particle->trample)
						return true;
				}			
			}
		}	
	}
	// enemy bullet
	chaos::box2 player_box = update_data.player->GetPlayerBox();
	player_box.half_size *= 0.7f;
	if (!particle->player_ownership && update_data.player != nullptr)
	{
		if (chaos::Collide(particle->bounding_box, player_box)) // destroy the particle outside the camera frustum (works for empty camera)
		{				
			update_data.player->SetLifeBarValue(-particle->damage, true);
			particle->damage = 0.0f;
			
			layer_trait->game->Play("player_touched", false, false, 0.0f, death::SoundContext::LEVEL);
		}	
	}

	// update position velocity
	particle->bounding_box.position += delta_time * particle->velocity;

	return false; // do not destroy the particle
}

size_t ParticleFireTrait::ParticleToVertices(ParticleFire const * particle, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const
{
	chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices, particle->rotation);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < 6; ++i)
		vertices[i].color = particle->color;

	float alpha = 1.0f;
	if (particle->lifetime < 1.0f)
		alpha = particle->lifetime;

	for (size_t i = 0; i < 6; ++i)
	{
		vertices[i].color = particle->color;
		vertices[i].color.a = alpha;
	}

	return 6;
}

// ===========================================================================
// ParticleEnemyTrait
// ===========================================================================

ParticleEnemyUpdateData ParticleEnemyTrait::BeginUpdateParticles(float delta_time, ParticleEnemy * particle, size_t count, LayerTrait const * layer_trait) const
{
	ParticleEnemyUpdateData result;
	if (count > 0)
	{
		result.camera_box = layer_trait->game->GetLudumLevelInstance()->GetCameraBox(0);
		//result.camera_box.half_size *= 3.0f;

		// search some bitmap layout information
		chaos::BitmapAtlas::FolderInfo const * bitmap_set = layer_trait->game->GetTextureAtlas()->GetFolderInfo("sprites");
		if (bitmap_set != nullptr)
		{
			chaos::BitmapAtlas::BitmapInfo const * fire_info = bitmap_set->GetBitmapInfo("enemy_fire");
			if (fire_info != nullptr)
				result.fire_layout = *fire_info;
		}	

		// get the layer of interrest
		death::TiledMap::LayerInstance * layer_instance = layer_trait->game->GetLudumLevelInstance()->FindLayerInstance("fire");
		if (layer_instance != nullptr)
		{
			chaos::ParticleLayerBase * fire_layer = layer_instance->GetParticleLayer();
			if (fire_layer != nullptr)
			{
				if (fire_layer->GetAllocationCount() > 0)
					result.fire_allocation = fire_layer->GetAllocation(0);
				else
					result.fire_allocation = fire_layer->SpawnParticles(0);
			}
		}
	}
	return result;
}

bool ParticleEnemyTrait::UpdateParticle(float delta_time, ParticleEnemy * particle, ParticleEnemyUpdateData const & update_data, LayerTrait const * layer_trait) const
{
	// destroy the particle if no life
	if (particle->life <= 0.0f)
		return true;
	// destroy the particle if outside a BIG camera box








	// shuxxx
	//if (!chaos::Collide(update_data.camera_box, particle->bounding_box)) // destroy the particle outside the camera frustum (works for empty camera)
	//	return true;	

	if (ObjectBesideCamera(update_data.camera_box, particle->bounding_box))
		return true;

	// apply velocity
	particle->bounding_box.position += delta_time * particle->velocity;
	// apply rotation
	
	if (particle->rotation_following_player)
	{
		LudumPlayer * player = layer_trait->game->GetLudumPlayer(0);
		if (player != nullptr)
		{		
			glm::vec2 delta_pos = player->GetPlayerPosition() - particle->bounding_box.position;
			particle->rotation = atan2f(delta_pos.y, delta_pos.x) - (float)M_PI_2; 
		}
	}
	else
	{
		particle->rotation += delta_time * particle->rotation_speed;	
	}

	// update blinking effect
	if (particle->touched_count_down > 0)
		--particle->touched_count_down;



	if (particle->fire_frequency > 0.0f)
	{
		particle->current_fire_timer += delta_time;
		if (particle->current_fire_timer >= particle->fire_frequency)
		{
			float size_ratio = 0.2f;
			int count = 4;
			float delta_angle = 2.0f * (float)M_PI / (float)count;
			ParticleFire * p = layer_trait->game->GetLudumGameInstance()->FireProjectile(update_data.fire_allocation.get(), particle->bounding_box, update_data.fire_layout, size_ratio, count, nullptr, delta_angle, false, layer_trait->game->enemy_fire_velocity, particle->rotation);
			if (p != nullptr)
			{
				for (int i = 0 ; i < count ; ++i)
				{
					p[i].damage = layer_trait->game->enemy_fire_damage;
					p[i].trample = false;
				}
			}
			particle->current_fire_timer = 0.0f;		
		}
	}

	return false; // do not destroy the particle
}

size_t ParticleEnemyTrait::ParticleToVertices(ParticleEnemy const * particle, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const
{
	chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices, particle->rotation);
	// select wanted color
	glm::vec4 color = particle->color;

	if (particle->touched_count_down > 0)
		color.a = 0.0f;
	else
		color.a = 1.0f;
	
	// copy the color in all triangles vertex
	for (size_t i = 0; i < 6; ++i)
		vertices[i].color = color;
	return 6;
}

