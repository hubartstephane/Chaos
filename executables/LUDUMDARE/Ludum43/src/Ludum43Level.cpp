#include "Ludum43Level.h"
#include "Ludum43Game.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>

// =============================================================
// AtomObject implementation
// =============================================================

AtomGameObject::AtomGameObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) :
	death::TiledMap::GeometricObject(in_layer_instance, in_geometric_object)
{
}

// =============================================================
// LudumLevel implementation
// =============================================================

#define LUDUM_PARTICLE_SPAWNER 1
#define LUDUM_PARTICLE_ENEMY 2

LudumLevel::LudumLevel()
{

}

death::GameLevelInstance * LudumLevel::DoCreateLevelInstance(death::Game * in_game)
{
	return new LudumLevelInstance(dynamic_cast<LudumGame *>(in_game));
}

death::TiledMap::GeometricObject * LudumLevel::DoCreateTypedObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object)
{
	int const * object_type_ptr = in_geometric_object->FindPropertyInt("OBJECT_TYPE");
	if (object_type_ptr == nullptr)
		return nullptr;

	int object_type = *object_type_ptr;

	if (object_type == LUDUM_PARTICLE_SPAWNER)
		return nullptr;
	if (object_type == LUDUM_PARTICLE_ENEMY)
		return nullptr;

	

	return nullptr;
}


chaos::ParticleLayer * LudumLevel::CreateParticleLayer(death::TiledMap::LayerInstance * layer_instance)
{
	std::string const & layer_name = layer_instance->GetTiledLayer()->name;
	if (layer_name == "PlayerAndCamera")
	{
		ParticlePlayerTrait trait;
		trait.game = dynamic_cast<LudumGame*>(layer_instance->GetGame());
		return new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<ParticlePlayerTrait>(trait));	
	}
	if (layer_name == "Enemies")
	{
		ParticleEnemyTrait trait;
		trait.game = dynamic_cast<LudumGame*>(layer_instance->GetGame());
		return new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<ParticleEnemyTrait>(trait));	

	}
	if (layer_name == "Atoms")
	{
		ParticleAtomTrait trait;
		trait.game = dynamic_cast<LudumGame*>(layer_instance->GetGame());
		return new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<ParticleAtomTrait>(trait));	
	}

	return new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<death::TiledMap::TileParticleTrait>);
}



// =============================================================
// LudumLevelInstance implementation
// =============================================================

LudumLevelInstance::LudumLevelInstance(LudumGame * in_game):
	game(in_game)
{
	assert(in_game != nullptr); 
}

LudumLevel * LudumLevelInstance::GetTypedLevel()
{
	return dynamic_cast<LudumLevel*>(GetLevel());
}

LudumLevel const * LudumLevelInstance::GetTypedLevel() const
{
	return dynamic_cast<LudumLevel const *>(GetLevel());
}
