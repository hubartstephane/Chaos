#include "Ludum50PCH.h"
#include "Ludum50Level.h"
#include "Ludum50LevelInstance.h"
#include "Ludum50Game.h"
#include "Ludum50Player.h"
#include "Ludum50GameInstance.h"
#include "Ludum50PlayerDisplacementComponent.h"


// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{
	level_instance_class = LudumLevelInstance::GetStaticClass();
}

ParticleLayerBase * LudumLevel::DoCreateParticleLayer(TMLayerInstance * layer_instance)
{
	LudumGame * ludum_game = auto_cast(layer_instance->GetGame());

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	if (StringTools::Stricmp(layer_name, "PlayerAndCamera") == 0)
	{
		ParticlePlayerLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new ParticleLayer<ParticlePlayerLayerTrait>(layer_trait);
	}




	return TMLevel::DoCreateParticleLayer(layer_instance);
}


TMObjectFactory LudumLevel::DoGetObjectFactory(TMLayerInstance * in_layer_instance, TiledMap::TypedObject const * in_typed_object)
{


#if 0
	if (in_typed_object->IsObjectOfType("Road"))
		return CHAOS_TM_MAKE_OBJECT_FACTORY(return new LudumRoad(););

	if (in_typed_object->IsObjectOfType("SpeedIndication"))
		return CHAOS_TM_MAKE_OBJECT_FACTORY(return new LudumSpeedIndication(););
#endif

	return TMLevel::DoGetObjectFactory(in_layer_instance, in_typed_object);
}


bool LudumLevel::Initialize(TiledMap::Map* in_tiled_map)
{
	if (!TMLevel::Initialize(in_tiled_map))
		return false;



	return true;
}
