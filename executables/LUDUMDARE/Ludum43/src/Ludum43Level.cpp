#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43Game.h"
#include "Ludum43Player.h"
#include "Ludum43GameInstance.h"

#include <chaos/Chaos.h>

// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{
	level_instance_class = LudumLevelInstance::GetStaticClass();
}

chaos::ParticleLayerBase * LudumLevel::DoCreateParticleLayer(death::TMLayerInstance * layer_instance)
{
	LudumGame * ludum_game = layer_instance->GetGame();

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	bool is_player_and_camera = (layer_name == "PlayerAndCamera");
	if (is_player_and_camera)
	{
		ParticlePlayerLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerLayerTrait>(layer_trait);
	}

	bool is_enemy = (layer_name == "Enemies");
	bool is_world_limit = (layer_name == "WorldLimits");
	if (is_enemy || is_world_limit)
	{
		ParticleEnemyLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		layer_trait.dynamic_particles = is_enemy; // shuxxx : optimization 
		layer_trait.dynamic_vertices = is_enemy;
		return new chaos::ParticleLayer<ParticleEnemyLayerTrait>(layer_trait);
	}

	bool is_atom = (layer_name == "Atoms");
	if (is_atom)
	{
		ParticleAtomLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleAtomLayerTrait>(layer_trait);
	}
	bool is_texts = (layer_name == "Texts");
	if (is_texts)
	{
		return new chaos::ParticleLayer<chaos::ParticleDefaultLayerTrait>();
	}

	return death::TMLevel::DoCreateParticleLayer(layer_instance);
}

static float GetWorldAndEnemyEffectiveRadius(float r, float factor, float offset)
{
	return r * factor + offset;
}

bool LudumLevel::FinalizeLayerParticles(death::TMLayerInstance * layer_instance, chaos::ParticleAllocationBase * allocation)
{
	if (!death::TMLevel::FinalizeLayerParticles(layer_instance, allocation))
		return false;

	LudumGame * ludum_game = layer_instance->GetGame();

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

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

	return true;
}

death::TMObjectFactory LudumLevel::DoGetObjectFactory(death::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object)
{
	if (death::TMTools::IsFinishTrigger(in_typed_object))
		return DEATH_MAKE_OBJECT_FACTORY(return new MyChangeLevelTrigger(););
	return death::TMLevel::DoGetObjectFactory(in_layer_instance, in_typed_object);
}

// =============================================================
// FinishingTriggerObject implementation
// =============================================================

bool MyChangeLevelTrigger::Initialize(death::TMLayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, death::TMObjectReferenceSolver& in_reference_solver)
{
	if (!death::TMTrigger::Initialize(in_layer_instance, in_geometric_object, in_reference_solver))
		return false;
	trigger_once = true;
	return true;
}

bool MyChangeLevelTrigger::OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type)
{
	death::Player* player = auto_cast(object);
	if (player == nullptr)
		return false;

	if (event_type != chaos::CollisionType::STARTED)
		return false;

	LudumPlayer* ludum_player = auto_cast(player);
	if (ludum_player != nullptr)
	{
		ParticlePlayer* player_particle = ludum_player->GetPlayerParticle();
		if (player_particle != nullptr)
		{
			if (!player_particle->level_end_reached)
			{
				player_particle->level_end_reached = true;
				player_particle->level_end_timer = 2.0f;
			}
		}
	}
	return true; // collisions handled successfully
}
