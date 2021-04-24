#include <chaos/Chaos.h>

#include "Ludum48Level.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48Game.h"
#include "Ludum48Player.h"
#include "Ludum48GameInstance.h"

// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{
	level_instance_class = LudumLevelInstance::GetStaticClass();
}

chaos::ParticleLayerBase * LudumLevel::DoCreateParticleLayer(chaos::TMLayerInstance * layer_instance)
{
	LudumGame * ludum_game = auto_cast(layer_instance->GetGame());

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	if (chaos::StringTools::Stricmp(layer_name, "PlayerAndCamera") == 0)
	{
		ParticlePlayerLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerLayerTrait>(layer_trait);
	}

	if ((chaos::StringTools::Stricmp(layer_name, "BurningFlames") == 0) || (chaos::StringTools::Stricmp(layer_name, "Animated") == 0))
	{
		return new chaos::ParticleLayer<ParticleAnimatedLayerTrait>();
	}

	return chaos::TMLevel::DoCreateParticleLayer(layer_instance);
}


chaos::TMObjectFactory LudumLevel::DoGetObjectFactory(chaos::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object)
{
	if (in_typed_object->IsObjectOfType("Spawner"))
	{
		std::string const* spawner_type = in_typed_object->FindPropertyString("SPAWNER_TYPE");
		if (spawner_type != nullptr)
		{

		}
	}

	return chaos::TMLevel::DoGetObjectFactory(in_layer_instance, in_typed_object);
}


bool LudumLevel::Initialize(chaos::TiledMap::Map* in_tiled_map)
{
	if (!chaos::TMLevel::Initialize(in_tiled_map))
		return false;


	return true;
}
