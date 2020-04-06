#include "LudumTestLevel.h"
#include "LudumTestLevelInstance.h"
#include "LudumTestGame.h"
#include "LudumTestPlayer.h"
#include "LudumTestGameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ParticleSpawner.h>

#include <death/TiledMapLevel.h>



// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{

}

death::LevelInstance * LudumLevel::DoCreateLevelInstance(death::Game * in_game)
{
	return new LudumLevelInstance(auto_cast(in_game));
}


chaos::ParticleLayerBase * LudumLevel::DoCreateParticleLayer(death::TiledMapLayerInstance * layer_instance)
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



	return death::TiledMapLevel::DoCreateParticleLayer(layer_instance);
}


death::GeometricObjectFactory LudumLevel::DoGetGeometricObjectFactory(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object)
{


	return death::TiledMapLevel::DoGetGeometricObjectFactory(in_layer_instance, in_geometric_object);
}

bool LudumLevel::OnPlayerTileCollision(float delta_time, class death::Player * player, death::TiledMapParticle * particle)
{

	return true; // continue with other
}
