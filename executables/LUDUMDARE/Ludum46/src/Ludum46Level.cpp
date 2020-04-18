#include "Ludum46Level.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46Game.h"
#include "Ludum46Player.h"
#include "Ludum46GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ParticleSpawner.h>

#include <death/TiledMapLevel.h>


// shuludum
//
// FindPropertyInt(...) mais return a value or a pointer depending on the additionnal DEFAULT parameter : not a good idea



// =============================================================
// LudumLevel implementation
// =============================================================

bool SpawnerObject::Initialize(chaos::TiledMap::GeometricObject* in_geometric_object)
{
	if (!death::TiledMapGeometricObject::Initialize(in_geometric_object))
		return false;

	max_spawned_particles = in_geometric_object->FindPropertyInt("MAX_SPAWNED_PARTICLES", max_spawned_particles);
	particle_start_velocity.x = in_geometric_object->FindPropertyFloat("PARTICLE_START_VELOCITY_X", particle_start_velocity.x);
	particle_start_velocity.y = in_geometric_object->FindPropertyFloat("PARTICLE_START_VELOCITY_Y", particle_start_velocity.y);


	return true;
}

bool SpawnerObject::DoTick(float delta_time)
{
	death::TiledMapGeometricObject::DoTick(delta_time);


	return true;
}
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

	if (chaos::StringTools::Stricmp(layer_name, "PlayerAndCamera") == 0)
	{
		ParticlePlayerTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerTrait>(layer_trait);
	}

	if (chaos::StringTools::Stricmp(layer_name, "Souls") == 0)
	{
		ParticleSoulTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleSoulTrait>(layer_trait);
	}

	if (chaos::StringTools::Stricmp(layer_name, "Fire") == 0)
	{
		ParticleFireTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleFireTrait>(layer_trait);
	}



	return death::TiledMapLevel::DoCreateParticleLayer(layer_instance);
}


death::GeometricObjectFactory LudumLevel::DoGetGeometricObjectFactory(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject* in_typed_object)
{
	if (chaos::TiledMapTools::IsObjectOfType(in_typed_object, "Spawner"))
		return DEATH_MAKE_GEOMETRICOBJECT_FACTORY(return new SpawnerObject(in_layer_instance););


	return death::TiledMapLevel::DoGetGeometricObjectFactory(in_layer_instance, in_typed_object);
}
