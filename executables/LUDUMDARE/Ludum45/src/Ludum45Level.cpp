#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Game.h"
#include "Ludum45Player.h"
#include "Ludum45GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ParticleSpawner.h>

#include <death/TM.h>


// =============================================================
// BonusSpawnerTrigger implementation
// =============================================================

bool BonusSpawnerTrigger::Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, death::TMObjectReferenceSolver& in_reference_solver)
{
	if (!death::TMTrigger::Initialize(in_layer_instance, in_geometric_object, in_reference_solver))
		return false;
	trigger_once = true;

	bonus_name = in_geometric_object->GetPropertyValueString("BONUS_TYPE", "");
	bonus_type = chaos::DeclareTag(bonus_name.c_str());

	return true;
}

// -------------------------------------------------------------------

bool BonusSpawnerTrigger::OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type)
{
	death::Camera* camera = auto_cast(object);
	if (camera == nullptr)
		return false;

	if (event_type != chaos::CollisionType::STARTED)
		return false;

	LudumGame * ludum_game = GetLayerInstance()->GetGame();
	if (ludum_game != nullptr)
	{
		LudumPlayer * ludum_player = ludum_game->GetPlayer(0);
		if (ludum_player != nullptr)
			ludum_player->OnPlayerUpgrade(bonus_type);	
	}
	return true;
}

// =============================================================
// EnemySpawnerTrigger implementation
// =============================================================

bool EnemySpawnerTrigger::Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, death::TMObjectReferenceSolver& in_reference_solver)
{
	if (!death::TMTrigger::Initialize(in_layer_instance, in_geometric_object, in_reference_solver))
		return false;
	trigger_once = true;

	enemy_type    = in_geometric_object->GetPropertyValueString("ENEMY_TYPE", "");
	enemy_pattern = in_geometric_object->GetPropertyValueString("ENEMY_PATTERN", "");


	// we possess the pattern
	pattern = new EnemyPattern;
	if (pattern != nullptr)
		pattern->Initialize(in_geometric_object);

	LudumGame * ludum_game = GetLayerInstance()->GetGame();
	if (ludum_game != nullptr)
		type = ludum_game->FindEnemyType(enemy_type.c_str());

	return true;
}

// -------------------------------------------------------------------

bool EnemySpawnerTrigger::OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type)
{
	death::Camera* camera = auto_cast(object);
	if (camera == nullptr)
		return false;

	if (event_type != chaos::CollisionType::STARTED)
		return false;

	if (type == nullptr || pattern == nullptr || pattern->enemy_count == 0)
		return true;

    // cast in a surface
	chaos::box2 spawner_box = GetBoundingBox(true);

    chaos::box2 pawn_box = GetLayerInstance()->GetGame()->GetPlayerPawn(0)->GetBox();

	LudumLevelInstance* ludum_level_instance = GetLayerInstance()->GetLevelInstance();

    chaos::ParticleSpawner spawner = ludum_level_instance->GetParticleSpawner("Enemies", type->bitmap_name.c_str());
    if (!spawner.IsValid())
        return true;

    chaos::BitmapAtlas::BitmapInfo const* bitmap_info = spawner.GetBitmapInfo();
  
    EnemyPattern * p = pattern;
    EnemyType* t = type;

	chaos::box2 camera_box = camera->GetCameraBox();

    spawner.SpawnParticles(pattern->enemy_count, true, [camera_box, pawn_box, spawner_box, t, p, bitmap_info](chaos::ParticleAccessorBase<ParticleEnemy> accessor) {

        for (ParticleEnemy& particle : accessor)
        {
            particle.bounding_box.half_size = 0.5f * glm::vec2(bitmap_info->width, bitmap_info->height) * t->scale;
            particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            particle.enemy_damage = t->enemy_damage;
            particle.enemy_health = t->enemy_health;
            particle.enemy_index = (int)(&particle - &accessor[0]);
            particle.enemy_particle_count = (int)accessor.GetCount();
            particle.pattern = p;
            particle.spawner_box = spawner_box;
            particle.time = 0.0f;
            p->UpdateParticle(0.0f, &particle, camera_box);
        }    
    });
    return true;
}

// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{
	level_instance_class = LudumLevelInstance::GetStaticClass();
}

chaos::ParticleLayerBase * LudumLevel::DoCreateParticleLayer(death::TMLayerInstance * layer_instance)
{
	LudumGame * ludum_game = layer_instance->GetGame();

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	bool is_player_and_camera = (chaos::StringTools::Stricmp(layer_name, "PlayerAndCamera") == 0);
	if (is_player_and_camera)
	{
		ParticlePlayerLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerLayerTrait>(layer_trait);
	}

	bool is_fire = (chaos::StringTools::Stricmp(layer_name, "PlayerFire") == 0) || (chaos::StringTools::Stricmp(layer_name, "EnemyFire") == 0);
	if (is_fire)
	{
		ParticleFireLayerTrait fire_trait;
		fire_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleFireLayerTrait>(fire_trait);
	}

	bool is_explosion = (chaos::StringTools::Stricmp(layer_name, "Explosions") == 0);
	if (is_explosion)
	{
		ParticleExplosionLayerTrait explosion_trait;
		explosion_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleExplosionLayerTrait>(explosion_trait);
	}


	bool is_bonus = (chaos::StringTools::Stricmp(layer_name, "bonus") == 0);
	if (is_bonus)
	{
		ParticleBonusLayerTrait bonus_trait;
		bonus_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleBonusLayerTrait>(bonus_trait);
	}

	bool is_enemies = (layer_name == "Enemies");
	if (is_enemies)
	{
		ParticleEnemyLayerTrait enemy_trait;
		enemy_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleEnemyLayerTrait>(enemy_trait);
	}

	return death::TMLevel::DoCreateParticleLayer(layer_instance);
}


death::TMObjectFactory LudumLevel::DoGetObjectFactory(death::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object)
{
	if (in_typed_object->IsObjectOfType("BONUS_SPAWNER"))
		return DEATH_MAKE_OBJECT_FACTORY(return new BonusSpawnerTrigger(););
	if (in_typed_object->IsObjectOfType("ENEMY_SPAWNER"))
		return DEATH_MAKE_OBJECT_FACTORY(return new EnemySpawnerTrigger(););
	return death::TMLevel::DoGetObjectFactory(in_layer_instance, in_typed_object);
}
