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

	return death::TiledMap::Level::CreateParticleLayer(layer_instance);
}


death::TiledMap::TriggerSurfaceObject * LudumLevel::DoCreateTriggerSurface(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object)
{
	if (in_geometric_object->name == "FinishSurface")
		return new FinishingTriggerSurfaceObject(in_layer_instance, in_geometric_object);
	if (in_geometric_object->name == "PowerUp")
		return new PowerUpTriggerSurfaceObject(in_layer_instance, in_geometric_object);

	return death::TiledMap::Level::DoCreateTriggerSurface(in_layer_instance, in_geometric_object);
}

// =============================================================
// FinishingTriggerSurfaceObject implementation
// =============================================================


FinishingTriggerSurfaceObject::FinishingTriggerSurfaceObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) :
	death::TiledMap::TriggerSurfaceObject(in_layer_instance, in_geometric_object)
{

}

bool FinishingTriggerSurfaceObject::OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type)
{
#if 0

	if (event_type == TriggerSurfaceObject::COLLISION_STARTED)
		event_type = event_type;
	if (event_type == TriggerSurfaceObject::COLLISION_FINISHED)
		event_type = event_type;
	if (event_type == TriggerSurfaceObject::COLLISION_AGAIN)
		event_type = event_type;


	if (player_particle != nullptr && event_type == TriggerSurfaceObject::COLLISION_STARTED)
	{
		ParticlePlayer * pp = (ParticlePlayer *)player_particle;
		if (!pp->level_end_reached)
		{
			//		pp->level_end_reached = true;
			//		pp->level_end_timer = 2.0f;
		}
	}
#endif
	return true; // continue other collisions
}


// =============================================================
// PowerUpTriggerSurfaceObject implementation
// =============================================================

PowerUpTriggerSurfaceObject::PowerUpTriggerSurfaceObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) :
	death::TiledMap::TriggerSurfaceObject(in_layer_instance, in_geometric_object)
{

}

bool PowerUpTriggerSurfaceObject::OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type)
{
#if 0

	if (event_type == TriggerSurfaceObject::COLLISION_STARTED)
		event_type = event_type;
	if (event_type == TriggerSurfaceObject::COLLISION_FINISHED)
		event_type = event_type;
	if (event_type == TriggerSurfaceObject::COLLISION_AGAIN)
		event_type = event_type;


	if (player_particle != nullptr && event_type == TriggerSurfaceObject::COLLISION_STARTED)
	{
		ParticlePlayer * pp = (ParticlePlayer *)player_particle;
		if (!pp->level_end_reached)
		{
			//		pp->level_end_reached = true;
			//		pp->level_end_timer = 2.0f;
		}
	}
#endif
	return true; // continue other collisions
}
