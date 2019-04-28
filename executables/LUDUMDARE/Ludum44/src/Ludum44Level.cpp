#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44Game.h"
#include "Ludum44Player.h"
#include "Ludum44GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>

// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{

}

death::GameLevelInstance * LudumLevel::DoCreateLevelInstance(death::Game * in_game)
{
	return new LudumLevelInstance(dynamic_cast<LudumGame *>(in_game));
}

bool LudumLevel::FinalizeLayerParticles(death::TiledMap::LayerInstance * layer_instance, chaos::ParticleAllocationBase * allocation)
{
	if (!death::TiledMap::Level::FinalizeLayerParticles(layer_instance, allocation))
		return false;



	return true;
}


chaos::ParticleLayerBase * LudumLevel::CreateParticleLayer(death::TiledMap::LayerInstance * layer_instance)
{
	LudumGame * ludum_game = dynamic_cast<LudumGame*>(layer_instance->GetGame());

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	bool is_player_and_camera = (layer_name == "PlayerAndCamera");
	if (is_player_and_camera)
	{
		ParticlePlayerTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerTrait>(layer_trait);
	}

	bool is_fire = (layer_name == "fire");
	if (is_fire)
	{
		ParticleFireTrait::LayerTrait fire_trait;
		fire_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleFireTrait>(fire_trait);
	}

	bool is_zone = (layer_name == "Zones");
	if (is_zone)
	{
		return new chaos::ParticleLayer<TileParticleTraitExt>();
	}

	bool is_enemies = (layer_name == "Enemies");
	if (is_enemies)
	{
		ParticleEnemyTrait::LayerTrait enemy_trait;
		enemy_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleEnemyTrait>(enemy_trait);
	}

	return death::TiledMap::Level::CreateParticleLayer(layer_instance);
}


death::TiledMap::TriggerSurfaceObject * LudumLevel::DoCreateTriggerSurface(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object)
{
	if (in_geometric_object->name == "Finish")
		return new FinishingTriggerSurfaceObject(in_layer_instance, in_geometric_object);
	if (in_geometric_object->name == "PowerUp")
		return new PowerUpTriggerSurfaceObject(in_layer_instance, in_geometric_object);
	if (in_geometric_object->name == "Checkpoint")
		return new CheckpointTriggerSurfaceObject(in_layer_instance, in_geometric_object);
	if (in_geometric_object->name == "SpeedUp")
		return new SpeedUpTriggerSurfaceObject(in_layer_instance, in_geometric_object);
	if (in_geometric_object->name == "Spawner")
		return new SpawnerTriggerSurfaceObject(in_layer_instance, in_geometric_object);

	return death::TiledMap::Level::DoCreateTriggerSurface(in_layer_instance, in_geometric_object);
}


bool LudumLevel::OnPlayerTileCollision(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, death::TiledMap::TileParticle * particle)
{

	return true; // continue with other
}

// =============================================================
// FinishingTriggerSurfaceObject implementation
// =============================================================

bool FinishingTriggerSurfaceObject::OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type)
{
	LudumLevelInstance * ludum_level_instance = dynamic_cast<LudumLevelInstance *>(GetLayerInstance()->GetTiledLevelInstance());
	if (ludum_level_instance == nullptr)
		return true;
	ludum_level_instance->SetLevelCompleted(true);

	return true; // continue other collisions
}


// =============================================================
// PowerUpTriggerSurfaceObject implementation
// =============================================================

bool PowerUpTriggerSurfaceObject::OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type)
{
	LudumGameInstance * ludum_game_instance = dynamic_cast<LudumGameInstance*>(player->GetGameInstance());
	if (ludum_game_instance != nullptr)
	{
		if (event_type == TriggerSurfaceObject::COLLISION_STARTED)
			ludum_game_instance->OnPowerUpZone(player, true, this);
		else if (event_type == TriggerSurfaceObject::COLLISION_FINISHED)
			ludum_game_instance->OnPowerUpZone(player, false, this);
	}

	return true; // continue other collisions
}

// =============================================================
// CheckPointTriggerSurfaceObject implementation
// =============================================================

bool CheckpointTriggerSurfaceObject::OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type)
{




	return false; // no more collisions
}

// =============================================================
// SpeedUpTriggerSurfaceObject implementation
// =============================================================

bool SpeedUpTriggerSurfaceObject::OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type)
{
	if (event_type != TriggerSurfaceObject::COLLISION_STARTED) // already handled
		return true;

	LudumLevelInstance * ludum_level_instance = dynamic_cast<LudumLevelInstance *>(GetLayerInstance()->GetTiledLevelInstance());
	if (ludum_level_instance == nullptr)
		return true;

	float scroll_speed = geometric_object->FindPropertyFloat("SCROLL_SPEED", 1.0f);
	ludum_level_instance->SetScrollFactor(scroll_speed);

	return true; // continue other collisions
}


// =============================================================
// SpawnerTriggerSurfaceObject implementation
// =============================================================

bool SpawnerTriggerSurfaceObject::OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type)
{
	// only the first time collision is detected
	if (event_type != death::TiledMap::TriggerSurfaceObject::COLLISION_STARTED)
		return true;

	// search the layer for enemies
	death::TiledMap::LayerInstance * enemy_layer_instance = GetLayerInstance()->GetTiledLevelInstance()->FindLayerInstance("Enemies");
	if (enemy_layer_instance == nullptr)
		return true;

	// search the atlas
	chaos::BitmapAtlas::TextureArrayAtlas const * atlas = enemy_layer_instance->GetGame()->GetTextureAtlas();
	if (atlas == nullptr)
		return true;

	// cast in a surface
	chaos::TiledMap::GeometricObjectSurface const * surface = geometric_object->GetObjectSurface();
	if (surface == nullptr)
		return true;

	// search BitmapLayout for Enemy
	chaos::BitmapAtlas::FolderInfo const * bitmap_set = atlas->GetFolderInfo("sprites");
	if (bitmap_set == nullptr)
		return true;
	
	chaos::BitmapAtlas::BitmapInfo const * enemy_info = bitmap_set->GetBitmapInfo("Enemy1");
	if (enemy_info == nullptr)
		return true;

	// create the particle layer if necessary
	if (enemy_layer_instance->CreateParticleLayer() == nullptr)
		return true;

	// create an allocation for all enemies we are about to create
	chaos::ParticleAllocationBase * allocation = enemy_layer_instance->CreateParticleAllocation();
	if (allocation == nullptr)
		return true;

	chaos::box2 surface_box = surface->GetBoundingBox(true);
	float scale_factor = surface->FindPropertyFloat("ENEMY_SCALE_FACTOR", 1.0f);
	int   count        = surface->FindPropertyInt("ENEMY_COUNT", 10);

	// Fill the enemies
	chaos::ParticleTexcoords texcoords = chaos::ParticleTools::GetParticleTexcoords(*enemy_info, atlas->GetAtlasDimension());

	if (allocation->AddParticles(count))
	{
		chaos::ParticleAccessor<ParticleEnemy> particles = allocation->GetParticleAccessor<ParticleEnemy>();
		for (int i = 0 ; i < count ; ++i)
		{
			particles[i].bounding_box.half_size = 0.5f * scale_factor * glm::vec2((float)enemy_info->width, (float)enemy_info->height);	
			particles[i].texcoords = texcoords;
			particles[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			particles[i].rotation_speed = 1.0f;

			particles[i].bounding_box.position = surface_box.position + (2.0f * chaos::GLMTools::RandVec2() - glm::vec2(1.0f, 1.0f)) * surface_box.half_size;





		}	
	}
	// auto delete allocation
	allocation->SetDestroyWhenEmpty(true);

	// disable the surface
	SetEnabled(false);

	return true; // continue other collisions
}
