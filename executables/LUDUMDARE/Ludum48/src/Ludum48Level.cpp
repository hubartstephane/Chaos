#include <chaos/Chaos.h>

#include "Ludum48.h"
#include "Ludum48Level.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48Game.h"
#include "Ludum48Player.h"
#include "Ludum48Particles.h"
#include "Ludum48GameInstance.h"

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
		layer_trait.tile_size = GetTiledMap()->tile_size;
		return new ParticleLayer<ParticlePlayerLayerTrait>(layer_trait);
	}

	if ((StringTools::Stricmp(layer_name, "BurningFlames") == 0) || (StringTools::Stricmp(layer_name, "Animated") == 0))
	{
		return new ParticleLayer<ParticleAnimatedLayerTrait>();
	}

	if (StringTools::Stricmp(layer_name, "GameObjects") == 0 || StringTools::Stricmp(layer_name, "Walls") == 0)
	{
		ParticleGameObjectLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		layer_trait.tile_size = GetTiledMap()->tile_size;
		return new ParticleLayer<ParticleGameObjectLayerTrait>(layer_trait);
	}

	if (StringTools::Stricmp(layer_name, "Gate") == 0)
	{
		ParticleGateLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		layer_trait.tile_size = GetTiledMap()->tile_size;
		return new ParticleLayer<ParticleGateLayerTrait>(layer_trait);
	}

	if (StringTools::Stricmp(layer_name, "Smoke") == 0)
	{
		ParticleSmokeLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		layer_trait.tile_size = GetTiledMap()->tile_size;
		return new ParticleLayer<ParticleSmokeLayerTrait>(layer_trait);
	}


	return TMLevel::DoCreateParticleLayer(layer_instance);
}


TMObjectFactory LudumLevel::DoGetObjectFactory(TMLayerInstance * in_layer_instance, TiledMap::TypedObject const * in_typed_object)
{
	if (in_typed_object->IsObjectOfType("Spawner"))
	{
		std::string const* spawner_type = in_typed_object->FindPropertyString("SPAWNER_TYPE");
		if (spawner_type != nullptr)
		{

		}
	}

	return TMLevel::DoGetObjectFactory(in_layer_instance, in_typed_object);
}


bool LudumLevel::Initialize(TiledMap::Map* in_tiled_map)
{
	if (!TMLevel::Initialize(in_tiled_map))
		return false;


	return true;
}


bool LudumLevel::FinalizeLayerParticles(TMLayerInstance* layer_instance, ParticleAllocationBase* allocation)
{
	if (!TMLevel::FinalizeLayerParticles(layer_instance, allocation))
		return false;

	if (layer_instance->GetCollisionMask() & COLLISION_GAMEOBJECT)
	{
		ParticleAccessor<GameObjectParticle> accessor = allocation->GetParticleAccessor();

		for (GameObjectParticle& particle : accessor)
		{
			TiledMap::TileInfo tile_info = GetTiledMap()->FindTileInfo(particle.gid);
			if (tile_info.tiledata != nullptr)
			{
				int const* object_type = tile_info.tiledata->FindPropertyInt("ObjectType");
				if (object_type != nullptr)
					particle.type = GameObjectType(*object_type);
			}
		}

	}
	return true;
}

