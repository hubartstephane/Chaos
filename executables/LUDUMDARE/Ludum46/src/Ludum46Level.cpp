#include "Ludum46Level.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46Game.h"
#include "Ludum46Player.h"
#include "Ludum46GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ParticleSpawner.h>
#include <chaos/StringTools.h>


#include <death/TiledMapLevel.h>


// shu46
//
// FindPropertyInt(...) mais return a value or a pointer depending on the additionnal DEFAULT parameter : not a good idea
// 
// SpawnParticle(...) maybe initialize automatically color to 1.0 (=> else 0 => invisible)
//
// chaos::ParticleAccessorBase<ParticleBase> => maybe some upcasting ?
//
// Level BoundingBox. Does not take care of tile/spawner very vwell rework on that

// creer un iterator/request qui permet de chercher toutes les collisions avec un type d objet donner par layer
//
//  layer->FindCollidingObject<SoulTriggerObject>(...);

// ca serait bien que dans
//
//    TiledMapLayerInstance::CreateGeometricObjectParticles(...) 
//
// on puisse recuperer les particules que l on vient de creer pour faire notre petite ttambouille ... pb : on utilise un cache de particles (et en plus elles sont typées en TiledMapParticle : aie)
//
//    TiledMapLayerInstanceParticlePopulator::AddParticle(
//
// eventuellement les objects geometrics pourraient avoir leur propre allocation !!!
//
//
///


//
// maybe review CheckCompletedLevel/ CanCompleteLevel / CheckGameOverCondition
// 

//
// UpdateParticle/DrawParticle : un prends un pointer l autre une ref !
// 

// Make ParticleTraits inherit one another / Particles inherit
//
//    class ParticleBurnedSoulTrait : public chaos::ParticleAllocationTrait<ParticleBurnedSoul, VertexBase>
//
//
// the WrapMode::Clamp is not working for GRID !!!
//
// Make a memo why
//  - there is a BurningFire.png in tileset
//  - there is a BurningFire_6x1 in sprites
//
// Make a memo how JSON textures are working
//
// Make a memo how JSON resources are working
//
//
// Refactor AtlasGeneration to make it much faster
//
//

// =============================================================
// EffectorObject implementation
// =============================================================

bool EffectorObject::Initialize(chaos::TiledMap::GeometricObject* in_geometric_object)
{


	return true;
}

// =============================================================
// SpikeBarObject implementation
// =============================================================

bool SpikeBarObject::Initialize(chaos::TiledMap::GeometricObject* in_geometric_object)
{
	if (!EffectorObject::Initialize(in_geometric_object))
		return false;
	if (!death::TiledMapGeometricObject::Initialize(in_geometric_object))
		return false;


	return true;
}

void SpikeBarObject::OnEffectorChangeState()
{
	active = active;


}

#if 0


delay_between_triggers = in_geometric_object->FindPropertyFloat("DELAY_BETWEEN_TRIGGERS", delay_between_triggers);

#endif

// =============================================================
// SoulTriggerObject implementation
// =============================================================

bool SoulTriggerObject::Initialize(chaos::TiledMap::GeometricObject* in_geometric_object)
{
	if (!death::TiledMapGeometricObject::Initialize(in_geometric_object))
		return false;

	// number of element that must be triggered
	trigger_limit = in_geometric_object->FindPropertyInt("TRIGGER_LIMIT", trigger_limit);

	// read all the effectors
	std::string effectors = in_geometric_object->FindPropertyString("EFFECTORS", "");

	char const separator = ',';

	std::vector<std::string> name_array = chaos::StringTools::Split(effectors.c_str(), separator);
	for (std::string & name : name_array)
		effector_names.push_back(std::move(name));

	// update internals (used whenever checkpoint is reloaded)
	trigger_count = 0;

	return true;
}

bool SoulTriggerObject::DoTick(float delta_time)
{
	death::TiledMapGeometricObject::DoTick(delta_time);

	for (std::string const& effector_name : effector_names)
	{
		EffectorObject* effector = auto_cast(layer_instance->FindGeometricObject(effector_name));
		if (effector == nullptr)
			continue;
		effector->SetEffectorState(trigger_count >= trigger_limit);
	}
	trigger_count = 0;

	return true;
}

bool SoulTriggerObject::AddTriggerCount()
{
	++trigger_count; // will be triggered next tick

	return false; // do not destroy the particle
}

// =============================================================
// SoulBurnTriggerObject implementation
// =============================================================

bool SoulBurnTriggerObject::AddTriggerCount()
{
	SoulTriggerObject::AddTriggerCount();

	LudumLevelInstance* ludum_level_instance = GetLayerInstance()->GetLevelInstance();
	if (ludum_level_instance != nullptr)
	{
		LudumPlayer* player = ludum_level_instance->GetPlayer(0);
		if (player != nullptr)
			player->AddBurnedSouls(1);

	}
	return true;
}








// =============================================================
// SpawnerObject implementation
// =============================================================

int SpawnerObject::GetRemainingParticleCount() const
{
	if (max_spawned_particles < 0) // infinite case
		return -1;
	return (max_spawned_particles - spawned_count);
}

bool SpawnerObject::Initialize(chaos::TiledMap::GeometricObject* in_geometric_object)
{
	if (!death::TiledMapGeometricObject::Initialize(in_geometric_object))
		return false;

	max_spawned_particles = in_geometric_object->FindPropertyInt("MAX_SPAWNED_PARTICLES", max_spawned_particles);
	particle_start_velocity.x = in_geometric_object->FindPropertyFloat("PARTICLE_START_VELOCITY_X", particle_start_velocity.x);
	particle_start_velocity.y = in_geometric_object->FindPropertyFloat("PARTICLE_START_VELOCITY_Y", particle_start_velocity.y);
	spawn_per_second = in_geometric_object->FindPropertyFloat("SPAWN_PER_SECOND", spawn_per_second);
	target_layer = in_geometric_object->FindPropertyString("TARGET_LAYER", "");
	spawned_particle = in_geometric_object->FindPropertyString("SPAWNED_PARTICLE", "");
	particle_duration = in_geometric_object->FindPropertyFloat("PARTICLE_DURATION", particle_duration);
	emission_started = in_geometric_object->FindPropertyBool("EMISSION_STARTED", emission_started);
	// update internals (used whenever checkpoint is reloaded)
	spawned_count = 0;
	nospawn_time_cumulated = 0.0f; 

	return true;
}

bool SpawnerObject::DoTick(float delta_time)
{
	death::TiledMapGeometricObject::DoTick(delta_time);

	// whether the emission is enabled 
	if (!emission_started)
		return true;
	// nothing to spawn
	if (spawn_per_second <= 0.0f)
		return true;
	// already all particles have been spawned
	if (max_spawned_particles > 0 && spawned_count >= max_spawned_particles)
		return true;

	// compute the number of particles to spawn
	int count = (int)(spawn_per_second * (delta_time + nospawn_time_cumulated));

	if (count == 0)
	{
		nospawn_time_cumulated += delta_time;
	}
	else
	{
		LudumLevelInstance * ludum_level_instance = GetLayerInstance()->GetLevelInstance();
		if (ludum_level_instance == nullptr)
			return true;

		chaos::ParticleSpawner spawner = ludum_level_instance->GetParticleSpawner(target_layer, spawned_particle);
		if (!spawner.IsValid())
			return false;

		SpawnParticles(spawner, count);
		spawned_count += count;
		nospawn_time_cumulated = 0.0f;
	}

	return true;
}

void SpawnerObject::SpawnParticles(chaos::ParticleSpawner & spawner, int count)
{	
	spawner.SpawnParticles(count, false, [this](chaos::ParticleAccessorBase<ParticleBase> accessor) 
	{
		chaos::box2 bx = GetBoundingBox(false);

		for (ParticleBase& p : accessor)
		{
			p.bounding_box = bx;
			p.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			p.velocity = particle_start_velocity;
			p.duration = particle_duration;
		}	
	});
}


// =============================================================
// SpawnerObject implementation
// =============================================================


bool FireSpawnerObject::Initialize(chaos::TiledMap::GeometricObject* in_geometric_object)
{
	if (!EffectorObject::Initialize(in_geometric_object))
		return false;
	if (!SpawnerObject::Initialize(in_geometric_object))
		return false;

	return true;
}

void FireSpawnerObject::OnEffectorChangeState()
{
	if (active)
		emission_started = true;
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

	if (chaos::StringTools::Stricmp(layer_name, "Blood") == 0)
	{
		ParticleBloodTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleBloodTrait>(layer_trait);
	}

	if (chaos::StringTools::Stricmp(layer_name, "BurnedSouls") == 0)
	{
		ParticleBurnedSoulTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleBurnedSoulTrait>(layer_trait);
	}

	if (chaos::StringTools::Stricmp(layer_name, "BurningFlames") == 0)
	{
		return new chaos::ParticleLayer<ParticleAnimatedTrait>();
	}




	return death::TiledMapLevel::DoCreateParticleLayer(layer_instance);
}


death::GeometricObjectFactory LudumLevel::DoGetGeometricObjectFactory(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject* in_typed_object)
{
	in_typed_object->type;

	if (chaos::TiledMapTools::IsObjectOfType(in_typed_object, "Spawner"))
	{
		std::string const* spawner_type = in_typed_object->FindPropertyString("SPAWNER_TYPE");
		if (spawner_type != nullptr)
		{
			if (chaos::StringTools::Strcmp(*spawner_type, "SoulSpawner") == 0)
				return DEATH_MAKE_GEOMETRICOBJECT_FACTORY(return new SoulSpawnerObject(in_layer_instance););
			if (chaos::StringTools::Strcmp(*spawner_type, "FireSpawner") == 0)
				return DEATH_MAKE_GEOMETRICOBJECT_FACTORY(return new FireSpawnerObject(in_layer_instance););
		}
	}

	if (chaos::TiledMapTools::IsObjectOfType(in_typed_object, "SoulTrigger"))
	{
		std::string const* trigger_type = in_typed_object->FindPropertyString("TRIGGER_TYPE");
		if (trigger_type != nullptr && chaos::StringTools::Strcmp(*trigger_type, "BurnTrigger") == 0)
			return DEATH_MAKE_GEOMETRICOBJECT_FACTORY(return new SoulBurnTriggerObject(in_layer_instance););
		else
			return DEATH_MAKE_GEOMETRICOBJECT_FACTORY(return new SoulTriggerObject(in_layer_instance););
	}
		
	if (chaos::TiledMapTools::IsObjectOfType(in_typed_object, "SpikeBar"))
		return DEATH_MAKE_GEOMETRICOBJECT_FACTORY(return new SpikeBarObject(in_layer_instance););

	return death::TiledMapLevel::DoGetGeometricObjectFactory(in_layer_instance, in_typed_object);
}


bool LudumLevel::Initialize(chaos::TiledMap::Map* in_tiled_map)
{
	if (!death::TiledMapLevel::Initialize(in_tiled_map))
		return false;

	required_souls = in_tiled_map->FindPropertyInt("REQUIRED_SOULS", 10);
	if (required_souls <= 0)
		return false;

	return true;
}
