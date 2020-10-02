#include "Ludum47Level.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47Game.h"
#include "Ludum47Player.h"
#include "Ludum47GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ParticleSpawner.h>
#include <chaos/StringTools.h>


#include <death/TM.h>





// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{
	level_instance_class = LudumLevelInstance::GetStaticClass();
}

chaos::ParticleLayerBase * LudumLevel::DoCreateParticleLayer(death::TMLayerInstance * layer_instance)
{
	LudumGame * ludum_game = auto_cast(layer_instance->GetGame());

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	if (chaos::StringTools::Stricmp(layer_name, "PlayerAndCamera") == 0)
	{
		ParticlePlayerLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerLayerTrait>(layer_trait);
	}




	return death::TMLevel::DoCreateParticleLayer(layer_instance);
}


death::TMObjectFactory LudumLevel::DoGetObjectFactory(death::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object)
{

	return death::TMLevel::DoGetObjectFactory(in_layer_instance, in_typed_object);
}


bool LudumLevel::Initialize(chaos::TiledMap::Map* in_tiled_map)
{
	if (!death::TMLevel::Initialize(in_tiled_map))
		return false;



	return true;
}
