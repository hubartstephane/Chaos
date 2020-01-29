#pragma once


#include "Ludum45Particles.h"
#include "Ludum45Game.h"
#include "Ludum45GameInstance.h"
#include "Ludum45Player.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Enemies.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>
#include <chaos/ParticleTools.h>

#include <death/SoundContext.h>


static float COLLISION_PLAYER_TWEAK = 0.50f;
static float COLLISION_FIRE_TWEAK = 0.75f;

static float OnCollisionWithEnemy(ParticleEnemy * enemy, float damage, LudumGame * game, bool collision_with_player, chaos::box2 const & ref_box) // returns the life damage produced by the enemy collision (its life)
{
	float result = collision_with_player? enemy->enemy_damage : enemy->enemy_health;

	// update life from both size
	enemy->enemy_health -= damage;
	enemy->touched_count_down = 0.05f;

	// play sound
	if (enemy->enemy_health > 0.0f)
		game->Play("metallic", false, false, 0.0f, death::SoundContext::LEVEL);
	else 
	{
		if (!collision_with_player)
			game->GetPlayer(0)->SetScore(10, true);
		game->Play("explosion", false, false, 0.0f, death::SoundContext::LEVEL);

		chaos::box2 b = ref_box;
		b.half_size *= 2.0f;

		game->GetLudumGameInstance()->FireExplosion(b);
	}
	return result;
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
					chaos::ParticleAccessor<ParticleEnemy> enemies = allocation->GetParticleAccessor();
					size_t count = enemies.GetCount();
					for (size_t j = 0 ; j < count ; ++j)
						result.push_back(&enemies[j]);
				}				
			}			
		}
	}
}

// ===========================================================================
// Utility
// ===========================================================================

static bool ObjectBesideCamera(chaos::box2 const & camera_box, chaos::box2 const & object_box)
{
	float cam_y = camera_box.position.y - camera_box.half_size.y;
	float obj_y = object_box.position.y + object_box.half_size.y;
	if (obj_y < cam_y)
		return true;
	return false;
}

// ===========================================================================
// ParticleEnemyTrait
// ===========================================================================

chaos::box2 ParticleEnemyTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleEnemy> & particle_accessor, LayerTrait const * layer_trait) const
{
	chaos::box2 result;

	result = layer_trait->game->GetPlayer(0)->GetPlayerBox();

	return result;
}

void ParticleEnemyTrait::ParticleToVertices(ParticleEnemy const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::TrianglePairPrimitive<VertexBase> primitive = output.AddPrimitive();

    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);

    // copy the color in all triangles vertex
    glm::vec4 color = particle.color;
    color.a = (particle.touched_count_down > 0.0f) ? 0.0f : 1.0f;
    
    for (size_t i = 0; i < 6; ++i)
        primitive[i].color = color;
}

void ParticleEnemyTrait::ParticleToVertices(ParticleEnemy const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);

    // copy the color in all triangles vertex
    glm::vec4 color = particle.color;
    color.a = (particle.touched_count_down > 0.0f) ? 0.0f : 1.0f;

    for (size_t i = 0; i < 4; ++i)
        primitive[i].color = color;
}

bool ParticleEnemyTrait::UpdateParticle(float delta_time, ParticleEnemy * particle, chaos::box2 const & player_box, LayerTrait const * layer_trait) const
{
	if (particle->enemy_health <= 0.0f)
		return true;

	chaos::box2 bb = particle->bounding_box;
	bb.half_size *= COLLISION_PLAYER_TWEAK;

	// collision with player

	// update blinking effect

	if (particle->touched_count_down > 0.0f)
		particle->touched_count_down -= delta_time;

	if (chaos::Collide(bb, player_box))
	{
		LudumPlayer * ludum_player = auto_cast(layer_trait->game->GetPlayer(0));
		if (ludum_player != nullptr)			
		{
			if (ludum_player->dash_timer <= 0.0f ||! ludum_player->GetGhostLevel())
			{
				float life_lost = OnCollisionWithEnemy(particle, particle->enemy_health, layer_trait->game, true, particle->bounding_box); // destroy the enemy always

				ludum_player->OnDamagedReceived(life_lost);
				
				return true;
			}
		}
	}

	// bitmap animation
	if (particle->bitmap_info != nullptr && particle->bitmap_info->HasGridAnimation())
	{
        float frame_time = particle->bitmap_info->GetFrameTime();

		particle->image_timer += delta_time;
		if (particle->image_timer > frame_time)
		{
			particle->image_timer = 0.0f;
			if (++particle->current_frame >= particle->bitmap_info->GetAnimationImageCount())
				particle->current_frame = 0;
		}
	}

	particle->time += delta_time;

	particle->bounding_box.position += particle->velocity * delta_time;

	
	if (particle->pattern != nullptr)
		return particle->pattern->UpdateParticle(delta_time, particle, player_box, layer_trait->game->GetLevelInstance()->GetCameraBox(0));


	return false;
}

// ===========================================================================
// ParticleBonusTrait
// ===========================================================================

#if 0

static void FindBonusOnMap(LudumGame * game, std::vector<ParticleBonus*> & result)
{
	// get the enemies
	death::TiledMap::LayerInstance * layer_instance = game->GetLudumLevelInstance()->FindLayerInstance("Bonus");
	if (layer_instance != nullptr)
	{
		chaos::ParticleLayerBase * layer = layer_instance->GetParticleLayer();
		if (layer != nullptr)
		{
			size_t allocation_count = layer->GetAllocationCount();
			for (size_t i = 0 ; i < allocation_count ; ++i)
			{
				chaos::ParticleAllocationBase * allocation = layer->GetAllocation(i);
				if (allocation != nullptr)
				{
					chaos::ParticleAccessor<ParticleBonus> bonus = allocation->GetParticleAccessor();
					size_t count = bonus.GetCount();
					for (size_t j = 0 ; j < count ; ++j)
						result.push_back(&bonus[j]);
				}				
			}			
		}
	}
}

#endif


chaos::box2 ParticleBonusTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleBonus>& particle_accessor, LayerTrait const * layer_trait) const
{
	chaos::box2 result;

	result = layer_trait->game->GetPlayer(0)->GetPlayerBox();

#if 0
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
#endif
	return result;
}

void ParticleBonusTrait::ParticleToVertices(ParticleBonus const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::TrianglePairPrimitive<VertexBase> primitive = output.AddPrimitive();

    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);
    // copy the color in all triangles vertex
    for (size_t i = 0; i < 6; ++i)
        primitive[i].color = particle.color;
}

void ParticleBonusTrait::ParticleToVertices(ParticleBonus const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();

    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);
    // copy the color in all triangles vertex
    for (size_t i = 0; i < 4; ++i)
        primitive[i].color = particle.color;
}

bool ParticleBonusTrait::UpdateParticle(float delta_time, ParticleBonus * particle, chaos::box2 const & player_box, LayerTrait const * layer_trait) const
{
	chaos::box2 bb = particle->bounding_box;
	bb.half_size *= 0.70f;

	if (chaos::Collide(bb, player_box))
	{
		LudumPlayer * ludum_player = auto_cast(layer_trait->game->GetPlayer(0));
		if (ludum_player != nullptr)
			ludum_player->OnPlayerUpgrade(particle->bonus_type);

		return true;
	}
	return false;
}

// ===========================================================================
// ParticlePlayerTrait
// ===========================================================================

void ParticlePlayerTrait::ParticleToVertices(ParticlePlayer const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();

    chaos::ParticleTexcoords texcoords = particle.texcoords;

    if (particle.bitmap_info != nullptr && particle.bitmap_info->HasGridAnimation())
    {
        chaos::BitmapAtlas::BitmapLayout layout = particle.bitmap_info->GetAnimationLayout(particle.current_frame, chaos::BitmapAtlas::GetBitmapLayoutFlag::wrap);

        texcoords = chaos::ParticleTools::GetParticleTexcoords(layout);
    }

    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, texcoords, primitive, particle.orientation);

    glm::vec4 boost_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    LudumPlayer const* player = layer_trait->game->GetLudumPlayer(0);
    if (player != nullptr && player->dash_timer > 0.0f && player->GetGhostLevel() > 0)
    {
        float alpha = 1.0f;

        if (layer_trait->game->player_dash_duration > 0.0f)
            alpha = player->dash_timer / layer_trait->game->player_dash_duration;

        alpha = 0.8f;

        boost_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f - alpha);
    }

    // copy the color in all triangles vertex
    for (size_t i = 0; i < 4; ++i)
        primitive[i].color = boost_color * particle.color;
}

void ParticlePlayerTrait::ParticleToVertices(ParticlePlayer const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::TrianglePairPrimitive<VertexBase> primitive = output.AddPrimitive();

    chaos::ParticleTexcoords texcoords = particle.texcoords;

    if (particle.bitmap_info != nullptr && particle.bitmap_info->HasGridAnimation())
    {
        chaos::BitmapAtlas::BitmapLayout layout = particle.bitmap_info->GetAnimationLayout(particle.current_frame, chaos::BitmapAtlas::GetBitmapLayoutFlag::wrap);

        texcoords = chaos::ParticleTools::GetParticleTexcoords(layout);
    }

    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, texcoords, primitive, particle.orientation);

    glm::vec4 boost_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    LudumPlayer const* player = layer_trait->game->GetLudumPlayer(0);
    if (player != nullptr && player->dash_timer > 0.0f && player->GetGhostLevel() > 0)
    {
        float alpha = 1.0f;

        if (layer_trait->game->player_dash_duration > 0.0f)
            alpha = player->dash_timer / layer_trait->game->player_dash_duration;

        alpha = 0.8f;

        boost_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f - alpha);
    }

    // copy the color in all triangles vertex
    for (size_t i = 0; i < 6; ++i)
        primitive[i].color = boost_color * particle.color;
}

bool ParticlePlayerTrait::UpdateParticle(float delta_time, ParticlePlayer * particle, LayerTrait const * layer_trait) const
{
	if (particle->bitmap_info != nullptr && particle->bitmap_info->HasGridAnimation())
	{
        float frame_time = particle->bitmap_info->GetFrameTime();

		particle->image_timer += delta_time;
		if (particle->image_timer > frame_time)
		{
			particle->image_timer = 0.0f;
			if (++particle->current_frame >= particle->bitmap_info->GetAnimationImageCount())
				particle->current_frame = 0;
		}
	}
	return false;
}

// ===========================================================================
// ParticleFireTrait
// ===========================================================================


ParticleFireUpdateData ParticleFireTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleFire>& particle_accessor, LayerTrait const * layer_trait) const
{
	ParticleFireUpdateData result;
	if (particle_accessor.GetCount() > 0)
	{
		// get the camera box 
		result.camera_box = layer_trait->game->GetLudumLevelInstance()->GetCameraBox(0);
		result.camera_box.half_size *= 1.1f;
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


	// search for collisions
	if (particle->player_ownership)
	{
		size_t count = update_data.enemies.size();
		for (size_t i = 0 ; i < count ; ++i)
		{
			ParticleEnemy * enemy = update_data.enemies[i];
			if (enemy->enemy_health > 0.0f)
			{
				chaos::box2 b1 = particle->bounding_box;
				chaos::box2 b2 = enemy->bounding_box;
				
				b1.half_size *= COLLISION_FIRE_TWEAK;
				b2.half_size *= COLLISION_FIRE_TWEAK;
				
				
				if (chaos::Collide(b1, b2))
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
			update_data.player->OnDamagedReceived(particle->damage);
			particle->damage = 0.0f;
		}	
	}

	// update position velocity
	particle->bounding_box.position += delta_time * particle->velocity;

	return false; // do not destroy the particle
}

void ParticleFireTrait::ParticleToVertices(ParticleFire const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::TrianglePairPrimitive<VertexBase> primitive = output.AddPrimitive();
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive, particle.rotation);

    // copy the color in all triangles vertex
    glm::vec4 color = particle.color;

    color.a = (particle.lifetime < 1.0f) ? particle.lifetime : 1.0f;
    for (size_t i = 0; i < 6; ++i)
        primitive[i].color = particle.color;
}

void ParticleFireTrait::ParticleToVertices(ParticleFire const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive, particle.rotation);

    // copy the color in all triangles vertex
    glm::vec4 color = particle.color;

    color.a = (particle.lifetime < 1.0f) ? particle.lifetime : 1.0f;
    for (size_t i = 0; i < 4; ++i)
        primitive[i].color = particle.color;
}


// ===========================================================================
// ParticleShroudLife
// ===========================================================================

bool ParticleShroudLifeTrait::UpdateParticle(float delta_time, ParticleShroudLife * particle, LayerTrait const * layer_trait) const
{
	if (particle->bitmap_info == nullptr)
		return false;
	LudumPlayer * ludum_player = layer_trait->game->GetLudumPlayer(0);
	if (ludum_player == nullptr)
		return false;

	float current_health = ludum_player->GetCurrentHealth();
	float max_health = ludum_player->GetCurrentMaxHealth();

	float image_count = (float)particle->bitmap_info->GetAnimationImageCount();

	int index = (int)(image_count * (1.0 - (current_health / max_health)));

	chaos::BitmapAtlas::BitmapLayout layout = particle->bitmap_info->GetAnimationLayout(index, chaos::BitmapAtlas::GetBitmapLayoutFlag::clamp);

	particle->texcoords = chaos::ParticleTools::GetParticleTexcoords(layout);


	return false; // never destroy it
}

void ParticleShroudLifeTrait::ParticleToVertices(ParticleShroudLife const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, output);
}

void ParticleShroudLifeTrait::ParticleToVertices(ParticleShroudLife const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, output);
}

// ===========================================================================
// ParticleLifeTrait
// ===========================================================================


// shuludum .... duplication AGAIN !!!

bool ParticleLifeTrait::UpdateParticle(float delta_time, ParticleLife * particle) const
{

	return false;
}

void ParticleLifeTrait::ParticleToVertices(ParticleLife const& particle, chaos::TrianglePairOutput<VertexBase>& output) const
{
    chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, output);
}

void ParticleLifeTrait::ParticleToVertices(ParticleLife const& particle, chaos::QuadOutput<VertexBase>& output) const
{
    chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, output);
}




// ===========================================================================
// ParticleExplosionTrait
// ===========================================================================


bool ParticleExplosionTrait::UpdateParticle(float delta_time, ParticleExplosion * particle, LayerTrait const * layer_trait) const
{
	if (particle->explosion_info == nullptr) // delete the particle
		return true;

	size_t image_count = particle->explosion_info->GetAnimationImageCount();
	float  frame_time = particle->explosion_info->GetFrameTime();

	if (frame_time == 0)
		frame_time = 1.0f / 16.0f;

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
	particle->texcoords = chaos::ParticleTools::GetParticleTexcoords(bitmap_layout);



	return false;
}

void ParticleExplosionTrait::ParticleToVertices(ParticleExplosion const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, output);
}

void ParticleExplosionTrait::ParticleToVertices(ParticleExplosion const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, output);

}


