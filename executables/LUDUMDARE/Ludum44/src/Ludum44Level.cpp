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


	return true; // continue other collisions
}
