#pragma once


#include "Ludum43Particles.h"
#include "Ludum43Game.h"
#include "Ludum43GameInstance.h"
#include "Ludum43Player.h"

#include <chaos/CollisionFramework.h>
#include <chaos/Class.h>

void GetTypedVertexDeclaration(chaos::GPUVertexDeclaration * result, boost::mpl::identity<VertexBase>)
{
	result->Push(chaos::VertexAttributeSemantic::POSITION, 0, chaos::VertexAttributeType::FLOAT2);
    result->Push(chaos::VertexAttributeSemantic::TEXCOORD, 0, chaos::VertexAttributeType::FLOAT3);
    result->Push(chaos::VertexAttributeSemantic::COLOR, 0, chaos::VertexAttributeType::FLOAT4);
    result->Push(chaos::VertexAttributeSemantic::POSITION, 1, chaos::VertexAttributeType::FLOAT2);
    result->Push(chaos::VertexAttributeSemantic::POSITION, 2, chaos::VertexAttributeType::FLOAT2); // center of the particle
}


// ===========================================================================
// Utility functions
// ===========================================================================

template<typename T>
static bool ApplyAffectorToParticles(float delta_time, T& particle, ParticleAffector const & affector, bool & in_inner_radius, float inner_radius_factor, glm::vec2 & result_velocity, float particle_radius_factor)
{
	result_velocity = glm::vec2(0.0f, 0.0f);

	glm::vec2 & particle_position = particle.bounding_box.position;
	glm::vec2 & particle_velocity = particle.velocity;

	glm::vec2 const & affector_position = affector.bounding_box.position;

	glm::vec2 delta_pos = (affector_position - particle_position);

	float l2 = glm::length2(delta_pos);

	float attraction_maxradius = particle_radius_factor * affector.attraction_maxradius;
	float attraction_minradius = particle_radius_factor * affector.attraction_minradius;

	// particle in range
	if (l2 < attraction_maxradius * attraction_maxradius)
	{
		float l = 0.0f;
		// particle never goes below lin radius
		if (attraction_minradius > 0.0f && l2 < attraction_minradius * attraction_minradius)
		{
			l = attraction_minradius;
			particle_position = affector_position - glm::normalize(delta_pos) * l;
		}
		else
		{
			l = std::sqrt(l2);
		}

		// whether the particle is inside the inner radius
		float inner_radius = attraction_minradius + inner_radius_factor * (attraction_maxradius - attraction_minradius);
		if (l <= inner_radius)
			in_inner_radius = true;

		float distance_ratio = 1.0f;
		distance_ratio = std::clamp(1.0f - (l - attraction_minradius) / (attraction_maxradius - attraction_minradius), 0.0f, 1.0f);

		// create a tangent force
		if (glm::length2(particle_velocity) > 0.0f)
		{
			glm::vec3 a = glm::normalize(glm::vec3(delta_pos.x, delta_pos.y, 0.0f));
			glm::vec3 b = glm::vec3(0.0f, 0.0f, 1.0f);
			glm::vec3 tangent = glm::cross(a, b);

			if (!affector.world_limits && !affector.reversed)
				result_velocity += distance_ratio * affector.tangent_force * glm::vec2(tangent.x, tangent.y);
		}

		if (!affector.world_limits)
		{
			if (!affector.reversed)
				result_velocity += distance_ratio *  affector.attraction_force * delta_pos;
			else
				result_velocity -= distance_ratio *  affector.repulsion_force * delta_pos;
		}

		return true;
	}
	return false;
}


template<typename T>
bool UpdateParticleLifeAndColor(T& particle, bool in_inner_radius, float delta_time, float lifetime, bool player, LudumGame * game)
{
	if (in_inner_radius)
	{
		particle.life = std::clamp(particle.life - delta_time, 0.0f, lifetime);
		if (particle.life <= 0.0f)
			return true; // destroy the particle
	}
	else
	{
		particle.life = std::clamp(particle.life + delta_time, 0.0f, lifetime);
	}

	if (player)
	{
		particle.color.a = 0.2f + 0.8f * (particle.life / lifetime);
	}
	else
	{
		particle.color.g = 1.0f - (particle.life / lifetime);
		particle.color.b = 1.0f - (particle.life / lifetime);
	}
	return false;
}

void UpdateVelocityAndPosition(float delta_time, ParticleBase& particle, bool apply_slowdown, float slowing_factor, float max_velocity)
{
	glm::vec2 & velocity = particle.velocity;
	glm::vec2 & position = particle.bounding_box.position;

	// particle slowing down
	if (apply_slowdown)
		velocity *= powf(slowing_factor, delta_time);

	// update particle velocity
	float velocity_length2 = glm::length2(particle.velocity);
	if (velocity_length2 > max_velocity * max_velocity)
		velocity = glm::normalize(velocity) * max_velocity;
	// update particle position
	position += velocity * delta_time;
}

// ===========================================================================
// ParticlePlayerTrait
// ===========================================================================

ParticlePlayerTrait::UpdatePlayerData ParticlePlayerTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticlePlayer>& particle_accessor, LayerTrait const * layer_trait) const
{
	ParticlePlayerTrait::UpdatePlayerData result;



	return result;
}


bool ParticlePlayerTrait::UpdateParticle(float delta_time, ParticlePlayer& particle, UpdatePlayerData const & update_data, LayerTrait const * layer_trait) const
{
	// search all nearby enemies
	std::vector<ParticleEnemy> enemy_particles;
	layer_trait->game->RegisterEnemiesInRange(particle.bounding_box.position, layer_trait->game->world_clamp_radius, enemy_particles, "Enemies", false);
	std::vector<ParticleEnemy> worldlimits_particles;
	layer_trait->game->RegisterEnemiesInRange(particle.bounding_box.position, layer_trait->game->world_clamp_radius, worldlimits_particles, "WorldLimits", true);

	// apply all effectors
	bool affected_by_enemies = false;
	bool affected_by_worldlimits = false;

	bool in_danger_zone = false;

	glm::vec2 sum_velocity = glm::vec2(0.0f, 0.0f);

	// enemies
	size_t enemy_count = enemy_particles.size();
	for (size_t i = 0; i < enemy_count; ++i)
	{
		glm::vec2 effector_velocity;
		affected_by_enemies |= ApplyAffectorToParticles(delta_time, particle, enemy_particles[i], in_danger_zone, DANGER_RADIUS_RATIO, effector_velocity, particle.particle_radius_factor);
		sum_velocity += effector_velocity;
	}

	// world
	size_t worldlimit_count = worldlimits_particles.size();
	for (size_t i = 0; i < worldlimit_count; ++i)
	{
		bool in_dontcare = false;
		glm::vec2 effector_velocity;
		affected_by_worldlimits |= ApplyAffectorToParticles(delta_time, particle, worldlimits_particles[i], in_dontcare, DANGER_RADIUS_RATIO, effector_velocity, 1.0f); // 1.0f => just to test
																																																																																		//sum_velocity += effector_velocity;
	}

	// loose life if your not in danger zone
	if (!particle.level_end_reached)
		if (!affected_by_worldlimits)
			in_danger_zone = true;


	particle.velocity += sum_velocity * 0.0f;

	// update life and color

	if (particle.level_end_reached)
	{
		particle.level_end_timer = std::max(0.0f, particle.level_end_timer - delta_time);
		particle.color.a = particle.level_end_timer / 2.0f;
	}
	else
	{

#if _DEBUG
		if (!layer_trait->game->GetCheatMode())
#endif
			if (UpdateParticleLifeAndColor(particle, in_danger_zone, delta_time, layer_trait->game->GetPlayer(0)->GetMaxHealth(), true, layer_trait->game))
				return false; // do not destroy the particle even if the life is 0 (but do not process the following code that displace it)
	}



	// add external forces
	float acceleration_factor = (particle.dash) ? 3.0f : 1.0f;

	//static float bbb = 5.0f;

	//if (chaos::GLMTools::Get2DCrossProductZ(particle.velocity, particle.acceleration) < 0.0f)
	//	acceleration_factor *= bbb;

	float max_velocity_factor = (particle.dash) ? 3.0f : 1.0f;

	particle.velocity += acceleration_factor * particle.acceleration * delta_time;

	// update and clamp the velocity
	bool apply_slowdown = (particle.acceleration == glm::vec2(0.0f, 0.0f));
	UpdateVelocityAndPosition(delta_time, particle, apply_slowdown, layer_trait->game->player_slowing_factor, max_velocity_factor * layer_trait->game->player_max_velocity);
	particle.acceleration = glm::vec2(0.0f, 0.0f);
	return false;
}

// ===========================================================================
// ParticleEnemyTrait
// ===========================================================================

void ParticleEnemyTrait::ParticleToPrimitives(ParticleEnemy const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const * layer_trait) const
{
    chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();

	chaos::ParticleToPrimitive(particle, primitive);

	for (size_t i = 0; i < primitive.count; ++i)
	{
        primitive[i].attraction_position =
			particle.bounding_box.position +
			2.0f * particle.attraction_maxradius * glm::normalize(primitive[i].position - particle.bounding_box.position);
        primitive[i].particle_center = particle.bounding_box.position;
	}
}

ParticleEnemyTrait::UpdateEnemyData ParticleEnemyTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleEnemy>& particle_accessor, LayerTrait const * layer_trait) const
{
	ParticleEnemyTrait::UpdateEnemyData result;


	return result;
}

bool ParticleEnemyTrait::UpdateParticle(float delta_time, ParticleEnemy& particle, ParticleEnemyTrait::UpdateEnemyData const & update_data, LayerTrait const * layer_trait) const
{
	particle.rotation_alpha += 2.0f * delta_time;
	if (particle.rotation_alpha > 2.0f * (float)M_PI)
		particle.rotation_alpha -= 2.0f * (float)M_PI;


	glm::vec2 v = glm::vec2(std::cos(particle.rotation_alpha), std::sin(particle.rotation_alpha));


	particle.bounding_box.position =
		particle.rotation_center +
		particle.rotation_radius * v;

	return false;
}


// ===========================================================================
// ParticleAtomTrait
// ===========================================================================

bool ParticleAtomTrait::UpdateParticle(float delta_time, ParticleAtom& particle, ParticleAtomTrait::UpdateAtomData const & update_data, LayerTrait const * layer_trait) const
{
	LudumGameInstance * ludum_game_instance = layer_trait->game->GetGameInstance();

	glm::vec2 const & player_position = update_data.player_particle.bounding_box.position;
	glm::vec2 & particle_position = particle.bounding_box.position;

	glm::vec2 delta_pos = (player_position - particle_position);

	float l2 = glm::length2(delta_pos);
	float l = 0.0f;

	// particle too far ?
	if (l2 > update_data.world_clamp_radius * update_data.world_clamp_radius)
		return false;

	// apply all effectors

	bool in_danger_zone = false;
	bool in_waken_up_zone = false;
	bool affected = false;

	glm::vec2 player_sum_velocity = glm::vec2(0.0f, 0.0f);

	affected |= ApplyAffectorToParticles(delta_time, particle, update_data.player_particle, in_waken_up_zone, WAKEN_RADIUS_RATIO, player_sum_velocity, particle.particle_radius_factor);

	if (in_waken_up_zone && !particle.waken_up)
	{
		if (ludum_game_instance != nullptr)
			ludum_game_instance->NotifyAtomCountChange(+1);
		particle.waken_up = true;
	}
	else if (!affected && particle.waken_up)
	{
		if (ludum_game_instance != nullptr)
			ludum_game_instance->NotifyAtomCountChange(-1);
		particle.waken_up = false;
	}

	// danger affectors
	glm::vec2 enemy_sum_velocity = glm::vec2(0.0f, 0.0f);

	size_t enemy_count = update_data.enemy_particles.size();
	for (size_t i = 0; i < enemy_count; ++i)
	{
		glm::vec2 enemy_velocity;
		affected |= ApplyAffectorToParticles(delta_time, particle, update_data.enemy_particles[i], in_danger_zone, DANGER_RADIUS_RATIO, enemy_velocity, particle.particle_radius_factor);
		enemy_sum_velocity += enemy_velocity;
	}

	particle.velocity += player_sum_velocity * 1.0f + enemy_sum_velocity * 1.0f;

	// update life and color
	if (UpdateParticleLifeAndColor(particle, in_danger_zone, delta_time, layer_trait->game->initial_particle_health, false, layer_trait->game))
	{
		if (particle.waken_up)
			if (ludum_game_instance != nullptr)
				ludum_game_instance->NotifyAtomCountChange(-1);
		return true;
	}

	bool apply_slowdown = !affected;
	UpdateVelocityAndPosition(delta_time, particle, apply_slowdown, update_data.slowing_factor, update_data.max_velocity);

	return false;
}

ParticleAtomTrait::UpdateAtomData ParticleAtomTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleAtom> & particle_accessor, LayerTrait const * layer_trait) const
{
	ParticleAtomTrait::UpdateAtomData result;

	

	LudumPlayer const* ludum_player = layer_trait->game->GetPlayer(0);
	if (ludum_player == nullptr)
		return result;

	ParticlePlayer const* player_particle = ludum_player->GetPlayerParticle();
	if (player_particle == nullptr)
		return result;

	result.player_particle = *player_particle;

	result.slowing_factor = layer_trait->game->particle_slowing_factor;
	result.max_velocity = layer_trait->game->particle_max_velocity;
	result.world_clamp_radius = layer_trait->game->world_clamp_radius;

	layer_trait->game->RegisterEnemiesInRange(result.player_particle.bounding_box.position, result.world_clamp_radius, result.enemy_particles, "Enemies", true);

	return result;
}

// ===========================================================================
// Object particle system
// ===========================================================================


bool ParticleLifeTrait::UpdateParticle(float delta_time, ParticleLife& particle, LayerTrait const * layer_trait) const
{
	return false;
}
