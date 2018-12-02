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
	LudumGame * ludum_game = dynamic_cast<LudumGame*>(layer_instance->GetGame());

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;
	if (layer_name == "PlayerAndCamera")
	{
		ParticlePlayerTrait trait;
		trait.game = ludum_game;
		return new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<ParticlePlayerTrait>(trait));	
	}
	if (layer_name == "Enemies")
	{
		ParticleEnemyTrait trait;
		trait.game = ludum_game;
		return new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<ParticleEnemyTrait>(trait));	

	}
	if (layer_name == "Atoms")
	{
		ParticleAtomTrait trait;
		trait.game = ludum_game;
		return new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<ParticleAtomTrait>(trait));	
	}

	return new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<death::TiledMap::TileParticleTrait>);
}

bool LudumLevel::FinalizeLayerParticles(death::TiledMap::LayerInstance * layer_instance, chaos::ParticleAllocation * allocation)
{
	LudumGame * ludum_game = dynamic_cast<LudumGame*>(layer_instance->GetGame());


	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	bool is_enemy = (layer_name == "Enemies");
	bool is_player = (layer_name == "PlayerAndCamera");

	if (is_enemy || is_player)
	{
		chaos::ParticleAccessor<ParticleAffector> particles = allocation->GetParticleAccessor<ParticleAffector>();

		size_t count = particles.GetCount();
		for (size_t i = 0 ; i < count ; ++i)
		{
			ParticleAffector & p = particles[i]; 
			if (is_player)
			{
				p.attraction_minradius = ludum_game->player_attraction_minradius;
				p.attraction_maxradius = ludum_game->player_attraction_maxradius;
				p.attraction_force     = ludum_game->player_attraction_force;
				p.tangent_force        = ludum_game->player_tangent_force;			
			}
			else
			{
				p.attraction_minradius = ludum_game->enemy_attraction_minradius;
				p.attraction_maxradius = ludum_game->enemy_attraction_maxradius;
				p.attraction_force     = ludum_game->enemy_attraction_force;
				p.tangent_force        = ludum_game->enemy_tangent_force;
				p.color                = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			}
		}	
		return true;
	}

	bool is_atoms = (layer_name == "Atoms");
	if (is_atoms)
	{
		chaos::ParticleAccessor<ParticleAtom> particles = allocation->GetParticleAccessor<ParticleAtom>();

		size_t count = particles.GetCount();
		for (size_t i = 0 ; i < count ; ++i)
		{
			ParticleAtom & p = particles[i]; 
			p.particle_radius_factor = chaos::MathTools::RandFloat(1.0f, ludum_game->particle_radius_rand_factor);
		}	
		return true;
	}


	return true;
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
