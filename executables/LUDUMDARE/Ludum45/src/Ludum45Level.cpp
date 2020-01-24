#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Game.h"
#include "Ludum45Player.h"
#include "Ludum45GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ParticleSpawner.h>

#include <death/TiledMapLevel.h>


// =============================================================
// BonusSpawnerTriggerObject implementation
// =============================================================

bool BonusSpawnerTriggerObject::IsAdditionalParticlesCreationEnabled() const
{
	return false;
}

bool BonusSpawnerTriggerObject::Initialize()
{
	if (!death::TiledMap::TriggerObject::Initialize())
		return false;
	trigger_once = true;

	bonus_name = geometric_object->FindPropertyString("BONUS_TYPE", "");
	bonus_type = chaos::MakeStaticTagType(bonus_name.c_str());

	return true;
}

// -------------------------------------------------------------------

bool BonusSpawnerTriggerObject::OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type)
{
	if (event_type != TriggerObject::COLLISION_STARTED)
		return false;

	LudumGame * ludum_game = auto_cast(GetLayerInstance()->GetGame());
	if (ludum_game != nullptr)
	{
		LudumPlayer * ludum_player = auto_cast(ludum_game->GetPlayer(0));
		if (ludum_player != nullptr)
			ludum_player->OnPlayerUpgrade(bonus_type);	
	}
	return true;
}

// =============================================================
// EnemySpawnerTriggerObject implementation
// =============================================================

bool EnemySpawnerTriggerObject::IsAdditionalParticlesCreationEnabled() const
{
	return false;
}

bool EnemySpawnerTriggerObject::Initialize()
{
	if (!death::TiledMap::TriggerObject::Initialize())
		return false;
	trigger_once = true;

	enemy_type    = geometric_object->FindPropertyString("ENEMY_TYPE", "");
	enemy_pattern = geometric_object->FindPropertyString("ENEMY_PATTERN", "");


	// we possess the pattern
	pattern = new EnemyPattern;
	if (pattern != nullptr)
		pattern->Initialize(geometric_object.get());

	LudumGame * ludum_game = auto_cast(GetLayerInstance()->GetGame());
	if (ludum_game != nullptr)
		type = ludum_game->FindEnemyType(enemy_type.c_str());

	return true;
}

// -------------------------------------------------------------------

bool EnemySpawnerTriggerObject::OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type)
{
	if (event_type != TriggerObject::COLLISION_STARTED)
		return false;

	if (type == nullptr || pattern == nullptr || pattern->enemy_count == 0)
		return true;

    // cast in a surface
    chaos::TiledMap::GeometricObjectSurface const* surface = geometric_object->GetObjectSurface();
    if (surface == nullptr)
        return true;

    death::TiledMap::LayerInstance * layer_instance = GetLayerInstance()->GetTiledLevelInstance()->FindLayerInstance("Enemies");
    if (layer_instance == nullptr)
        return true;
    
    chaos::box2 player_box = GetLayerInstance()->GetGame()->GetPlayer(0)->GetPlayerBox();

    chaos::ParticleSpawner spawner = layer_instance->GetParticleSpawner(type->bitmap_name.c_str());
    if (!spawner.IsValid())
        return true;

    chaos::BitmapAtlas::BitmapInfo const* bitmap_info = spawner.GetBitmapInfo();
  
    EnemyPattern * p = pattern;
    EnemyType* t = type;

    spawner.SpawnParticles(pattern->enemy_count, true, [camera_box, player_box, surface, t, p, bitmap_info](chaos::ParticleAccessorBase<ParticleEnemy> accessor) {

        for (ParticleEnemy& particle : accessor)
        {
            particle.bounding_box.half_size = 0.5f * glm::vec2(bitmap_info->width, bitmap_info->height) * t->scale;
            particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            particle.enemy_damage = t->enemy_damage;
            particle.enemy_life = t->enemy_life;
            particle.enemy_index = (int)(&particle - &accessor[0]);
            particle.enemy_particle_count = (int)accessor.GetCount();
            particle.pattern = p;
            particle.spawner_surface = surface->GetBoundingBox(true);
            particle.time = 0.0f;
            p->UpdateParticle(0.0f, &particle, player_box, camera_box);
        }    
    });
    return true;
}

// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{

}

death::GameLevelInstance * LudumLevel::DoCreateLevelInstance(death::Game * in_game)
{
	return new LudumLevelInstance(auto_cast(in_game));
}

bool LudumLevel::FinalizeLayerParticles(death::TiledMap::LayerInstance * layer_instance, chaos::ParticleAllocationBase * allocation)
{
	if (!death::TiledMap::Level::FinalizeLayerParticles(layer_instance, allocation))
		return false;

	return true;
}


chaos::ParticleLayerBase * LudumLevel::DoCreateParticleLayer(death::TiledMap::LayerInstance * layer_instance)
{
	LudumGame * ludum_game = auto_cast(layer_instance->GetGame());

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	bool is_player_and_camera = (chaos::StringTools::Stricmp(layer_name, "PlayerAndCamera") == 0);
	if (is_player_and_camera)
	{
		ParticlePlayerTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerTrait>(layer_trait);
	}

	bool is_fire = (chaos::StringTools::Stricmp(layer_name, "PlayerFire") == 0) || (chaos::StringTools::Stricmp(layer_name, "EnemyFire") == 0);
	if (is_fire)
	{
		ParticleFireTrait::LayerTrait fire_trait;
		fire_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleFireTrait>(fire_trait);
	}

	bool is_explosion = (chaos::StringTools::Stricmp(layer_name, "Explosions") == 0);
	if (is_explosion)
	{
		ParticleExplosionTrait::LayerTrait explosion_trait;
		explosion_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleExplosionTrait>(explosion_trait);
	}


	bool is_bonus = (chaos::StringTools::Stricmp(layer_name, "bonus") == 0);
	if (is_bonus)
	{
		ParticleBonusTrait::LayerTrait bonus_trait;
		bonus_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleBonusTrait>(bonus_trait);
	}

	bool is_enemies = (layer_name == "Enemies");
	if (is_enemies)
	{
		ParticleEnemyTrait::LayerTrait enemy_trait;
		enemy_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleEnemyTrait>(enemy_trait);
	}

	return death::TiledMap::Level::DoCreateParticleLayer(layer_instance);
}


death::TiledMap::GeometricObject * LudumLevel::DoCreateGeometricObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object)
{
	chaos::TiledMap::GeometricObjectSurface * surface_object = in_geometric_object->GetObjectSurface();

	if (surface_object != nullptr)
	{
		if (chaos::TiledMapTools::HasFlag(surface_object, nullptr, nullptr, "BONUS_SPAWNER")) // shuludum : it is better to rely on a type of an object that to rely on its name
			return new BonusSpawnerTriggerObject(in_layer_instance, surface_object);          //            here we test for the property 'BONUS_SPAWNER' . Maybe the cleaner. Beware HasFlag method does not recurse like FindPropery(...)

		if (chaos::TiledMapTools::HasFlag(surface_object, nullptr, nullptr, "ENEMY_SPAWNER"))
			return new EnemySpawnerTriggerObject(in_layer_instance, surface_object);
	}
	return death::TiledMap::Level::DoCreateGeometricObject(in_layer_instance, in_geometric_object);
}

bool LudumLevel::OnPlayerTileCollision(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, death::TiledMap::TileParticle * particle)
{

	return true; // continue with other
}
