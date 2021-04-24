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


bool LudumLevel::FinalizeLayerParticles(chaos::TMLayerInstance* layer_instance, chaos::ParticleAllocationBase* allocation)
{
	if (!chaos::TMLevel::FinalizeLayerParticles(layer_instance, allocation))
		return false;

	if (layer_instance->GetCollisionMask() & COLLISION_GAMEOBJECT)
	{
		chaos::ParticleAccessor<GameObjectParticle> accessor = allocation->GetParticleAccessor();

		for (GameObjectParticle& particle : accessor)
		{

		}

	}


#if 0


	LudumGame* ludum_game = layer_instance->GetGame();

	std::string const& layer_name = layer_instance->GetTiledLayer()->name;

	bool is_enemy = (layer_name == "Enemies");
	bool is_world_limits = (layer_name == "WorldLimits");

	if (is_enemy || is_world_limits)
	{
		chaos::ParticleAccessor<ParticleEnemy> particles = allocation->GetParticleAccessor();
		for (ParticleEnemy& p : particles)
		{
			chaos::sphere2 c = chaos::GetInnerSphere(p.bounding_box);

			float& radius = c.radius;

			// XXX : i should rework on level themselves instead of scale 1.2 for world limits
			if (!is_world_limits)
				radius *= chaos::MathTools::RandFloat(0.8f, 2.0f);
			else
				radius *= 1.2f;

			p.bounding_box = chaos::GetBoundingBox(c);

			if (is_enemy)
			{
				p.attraction_minradius = GetWorldAndEnemyEffectiveRadius(radius, ludum_game->enemy_attraction_minradius_factor, ludum_game->enemy_attraction_minradius_offset);
				p.attraction_maxradius = GetWorldAndEnemyEffectiveRadius(radius, ludum_game->enemy_attraction_maxradius_factor, ludum_game->enemy_attraction_maxradius_offset);
			}
			else
			{
				p.attraction_minradius = GetWorldAndEnemyEffectiveRadius(radius, ludum_game->worldlimits_attraction_minradius_factor, ludum_game->worldlimits_attraction_minradius_offset);
				p.attraction_maxradius = GetWorldAndEnemyEffectiveRadius(radius, ludum_game->worldlimits_attraction_maxradius_factor, ludum_game->worldlimits_attraction_maxradius_offset);
			}
			p.attraction_force = ludum_game->enemy_attraction_force;
			p.repulsion_force = ludum_game->enemy_repulsion_force;
			p.tangent_force = ludum_game->enemy_tangent_force;
			p.reversed = is_world_limits;
			p.world_limits = is_world_limits;
			p.color = (is_world_limits) ?
				glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) :
				glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			p.rotation_center = p.bounding_box.position;
			if (is_enemy)
			{
				p.rotation_radius = radius * chaos::MathTools::RandFloat(0.0f, 2.0f);
				p.rotation_alpha = chaos::MathTools::RandFloat(0.0f, 6.28f);
			}
		}
		return true;
	}

	bool is_atoms = (layer_name == "Atoms");
	if (is_atoms)
	{
		chaos::ParticleAccessor<ParticleAtom> particles = allocation->GetParticleAccessor();
		for (ParticleAtom& p : particles)
		{
			p.particle_radius_factor = chaos::MathTools::RandFloat(ludum_game->particle_min_radius_factor, ludum_game->particle_max_radius_factor);
			p.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			p.life = ludum_game->initial_particle_health;
		}
		return true;
	}


#endif

	return true;
}




