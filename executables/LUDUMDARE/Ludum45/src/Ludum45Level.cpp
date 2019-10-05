#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Game.h"
#include "Ludum45Player.h"
#include "Ludum45GameInstance.h"

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
	return new LudumLevelInstance(auto_cast(in_game));
}

bool LudumLevel::FinalizeLayerParticles(death::TiledMap::LayerInstance * layer_instance, chaos::ParticleAllocationBase * allocation)
{
	if (!death::TiledMap::Level::FinalizeLayerParticles(layer_instance, allocation))
		return false;



	return true;
}


chaos::ParticleLayerBase * LudumLevel::CreateParticleLayer(death::TiledMap::LayerInstance * layer_instance)
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

	bool is_fire = (chaos::StringTools::Stricmp(layer_name, "fire") == 0);
	if (is_fire)
	{
		ParticleFireTrait::LayerTrait fire_trait;
		fire_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleFireTrait>(fire_trait);
	}

#if 0
	bool is_enemies = (layer_name == "Enemies");
	if (is_enemies)
	{
		ParticleEnemyTrait::LayerTrait enemy_trait;
		enemy_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleEnemyTrait>(enemy_trait);
	}
#endif















	return death::TiledMap::Level::CreateParticleLayer(layer_instance);
}


death::TiledMap::GeometricObject * LudumLevel::DoCreateGeometricObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object)
{
	chaos::TiledMap::GeometricObjectSurface * surface_object = in_geometric_object->GetObjectSurface();

	if (surface_object != nullptr)
	{

	}
	return death::TiledMap::Level::DoCreateGeometricObject(in_layer_instance, in_geometric_object);
}


bool LudumLevel::OnPlayerTileCollision(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, death::TiledMap::TileParticle * particle)
{

	return true; // continue with other
}
