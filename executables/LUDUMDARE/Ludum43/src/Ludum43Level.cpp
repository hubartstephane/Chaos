#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43Game.h"
#include "Ludum43Player.h"
#include "Ludum43GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>

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
	LudumGame * ludum_game = layer_instance->GetGame();

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	bool is_player_and_camera = (layer_name == "PlayerAndCamera");
	if (is_player_and_camera)
	{
		ParticlePlayerTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerTrait>(layer_trait);
	}

	bool is_enemy = (layer_name == "Enemies");
	bool is_world_limit = (layer_name == "WorldLimits");
	if (is_enemy || is_world_limit)
	{
		ParticleEnemyTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		layer_trait.dynamic_particles = is_enemy; // shuxxx : optimization 
		layer_trait.dynamic_vertices = is_enemy;
		return new chaos::ParticleLayer<ParticleEnemyTrait>(layer_trait);
	}

	bool is_atom = (layer_name == "Atoms");
	if (is_atom)
	{
		ParticleAtomTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleAtomTrait>(layer_trait);
	}
	bool is_texts = (layer_name == "Texts");
	if (is_texts)
	{
		return new chaos::ParticleLayer<chaos::ParticleDefault::ParticleTrait>();
	}

	return death::TiledMapLevel::DoCreateParticleLayer(layer_instance);
}

static float GetWorldAndEnemyEffectiveRadius(float r, float factor, float offset)
{
	return r * factor + offset;
}

bool LudumLevel::FinalizeLayerParticles(death::TiledMapLayerInstance * layer_instance, chaos::ParticleAllocationBase * allocation)
{

	// shufinalize



	LudumGame * ludum_game = layer_instance->GetGame();


	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	bool is_enemy = (layer_name == "Enemies");
	bool is_world_limits = (layer_name == "WorldLimits");
	bool is_player = (layer_name == "PlayerAndCamera");

	if (is_player)
	{
		chaos::ParticleAccessor<ParticleAffector> particles = allocation->GetParticleAccessor();

		size_t count = particles.GetCount();
		for (size_t i = 0; i < count; ++i)
		{
			ParticleAffector & p = particles[i];

			float radius = chaos::GetInnerSphere(p.bounding_box).radius;

			p.attraction_minradius = radius + ludum_game->player_attraction_minradius;
			p.attraction_maxradius = radius + ludum_game->player_attraction_maxradius;
			p.attraction_force = ludum_game->player_attraction_force;
			p.repulsion_force = ludum_game->player_repulsion_force;
			p.tangent_force = ludum_game->player_tangent_force;
		}
		return true;
	}


	if (is_enemy || is_world_limits)
	{
		chaos::ParticleAccessor<ParticleEnemy> particles = allocation->GetParticleAccessor();

		size_t count = particles.GetCount();
		for (size_t i = 0; i < count; ++i)
		{
			ParticleEnemy & p = particles[i];

			chaos::sphere2 c = chaos::GetInnerSphere(p.bounding_box);

			float & radius = c.radius;

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

		size_t count = particles.GetCount();
		for (size_t i = 0; i < count; ++i)
		{
			ParticleAtom & p = particles[i];
			p.particle_radius_factor = chaos::MathTools::RandFloat(ludum_game->particle_min_radius_factor, ludum_game->particle_max_radius_factor);
			p.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			p.life = ludum_game->initial_particle_health;
		}
		return true;
	}

	return true;
}

death::TiledMapGeometricObject * LudumLevel::DoCreateGeometricObject(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object)
{
	chaos::TiledMap::GeometricObjectSurface * surface_object = in_geometric_object->GetObjectSurface();

	if (surface_object != nullptr)
	{
		if (chaos::TiledMapTools::HasFlag(surface_object, "FinishSurface", "FinishSurface", "FINISHSURFACE"))
			return new MyFinishingTriggerObject(in_layer_instance, surface_object);
	}
	return death::TiledMapLevel::DoCreateGeometricObject(in_layer_instance, in_geometric_object);
}

// =============================================================
// FinishingTriggerObject implementation
// =============================================================

MyFinishingTriggerObject::MyFinishingTriggerObject(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::GeometricObjectSurface * in_surface_object) :
	death::TiledMapTriggerObject(in_layer_instance, in_surface_object)
{

}

bool MyFinishingTriggerObject::Initialize()
{
	if (!death::TiledMapTriggerObject::Initialize())
		return false;
	trigger_once = true;
	return true;
}


bool MyFinishingTriggerObject::OnPlayerCollisionEvent(float delta_time, death::Player * player, chaos::CollisionType event_type)
{
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
