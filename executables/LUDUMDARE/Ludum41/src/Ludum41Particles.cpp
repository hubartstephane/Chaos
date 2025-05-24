#pragma once

#include "Ludum41PCH.h"
#include "Ludum41Particles.h"
#include "Ludum41Game.h"
#include "Ludum41GameInstance.h"
#include "Ludum41LevelInstance.h"
#include "Ludum41Challenge.h"

// ===========================================================================
// Object particle system
// ===========================================================================

bool ParticleObjectLayerTrait::UpdateParticle(float delta_time, ParticleObject& particle) const
{
	return false;
}

// ===========================================================================
// Life particle system
// ===========================================================================

int ParticleLifeLayerTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleObject>& particle_accessor) const
{
	return 0;
}

bool ParticleLifeLayerTrait::UpdateParticle(float delta_time, ParticleObject& particle, int extra_param) const
{
	return false;
}

// ===========================================================================
// Brick particle system
// ===========================================================================

bool ParticleBrickLayerTrait::UpdateParticle(float delta_time, ParticleBrick& particle) const
{
	if (particle.life <= 0)
		return true;

	return false;
}

void ParticleBrickLayerTrait::ParticleToPrimitives(ParticleBrick const& particle, chaos::GPUPrimitiveOutput<VertexBase>& output) const
{
    LudumGameInstance const* ludum_game_instance = game->GetGameInstance();

	ParticleBrick other = particle;

	// tweak bounding box
	other.bounding_box.position.y -= ludum_game_instance->brick_offset;

	// tweak color
	float extra = 2;
	float ratio = (extra + other.life) / (extra + other.starting_life);
	other.color = ratio * other.color;

	chaos::ParticleToPrimitives(other, output);
}

// ===========================================================================
// Object Movable particle system
// ===========================================================================

void ParticleMovableObjectLayerTrait::ParticleToPrimitives(ParticleMovableObject const& particle, chaos::GPUPrimitiveOutput<VertexBase>& output) const
{
    LudumGameInstance const* ludum_game_instance = game->GetGameInstance();

	ParticleMovableObject other = particle;

	// tweak color
	float ball_power = ludum_game_instance->ball_power;

	glm::vec4 power_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	if (ball_power <= 0.5f)
		power_color = glm::vec4(0.0f, 0.58f, 1.0f, 1.0f);
	else if (ball_power == 2.0f)
		power_color = glm::vec4(1.0f, 0.41f, 0.0f, 1.0f);
	else if (ball_power >= 3.0f)
		power_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	other.color = other.color * power_color;

	chaos::ParticleToPrimitives(other, output);
}

void ParticleMovableObjectLayerTrait::UpdateParticleVelocityFromCollision(glm::vec2 const & old_position, glm::vec2 const & new_position, glm::vec2 & velocity) const
{
	size_t dimension = velocity.length();

	for (size_t i = 0; i < dimension; ++i)
	{
		if (old_position[i] > new_position[i])
			velocity[i] = -abs(velocity[i]);
		else if (old_position[i] < new_position[i])
			velocity[i] = abs(velocity[i]);
	}
}

glm::vec2 MakeVelocityFromAngle(float angle)
{
	return glm::vec2(std::cos(angle), std::sin(angle));
}

bool ParticleMovableObjectLayerTrait::UpdateParticle(float delta_time, ParticleMovableObject& particle) const
{
	LudumGameInstance * game_instance = game->GetGameInstance();
	if (game_instance == nullptr)
		return false;

	// do not update particles during pause
	if (!game->IsPlaying() || game->IsFreeCameraMode())
		return false;

	// delay before moving the particle
	if (particle.delay_before_move > 0.0f)
	{
		particle.delay_before_move -=delta_time;
		if (particle.delay_before_move > 0.0f)
			return false;
	}

	// update the velocity of the ball
	glm::vec2 velocity = glm::normalize(particle.velocity);

	// moving the particle
	particle.bounding_box.position += velocity *
		(game_instance->ball_collision_speed + game_instance->ball_speed) *
		(delta_time * game_instance->ball_time_dilation);

	// ball bouncing against world

	chaos::box2 canvas_box = game->GetCanvasBox();
	chaos::box2 ball_box = particle.bounding_box;


	// bounce against the world borders
	chaos::box2 new_ball_box = ball_box;
	if (chaos::RestrictToInside(canvas_box, new_ball_box, false))
	{
		glm::vec2 old_velocity = velocity;

		chaos::UpdateVelocityFromCollision(ball_box.position, new_ball_box.position, velocity);
		ball_box.position = new_ball_box.position;

		if (old_velocity.y < 0.0f && velocity.y > 0.0f)
		{

			return true; // ball lost
		}
	}

	// bounce against player
	chaos::PlayerPawn * player_pawn = game->GetPlayerPawn(0);
	if (player_pawn == nullptr)
		return false;

	if (player_pawn != nullptr)
	{
		chaos::box2 pawn_box = player_pawn->GetBoundingBox();

		chaos::box2 new_ball_box = ball_box;
		if (chaos::RestrictToOutside(pawn_box, new_ball_box))
		{
			chaos::UpdateVelocityFromCollision(ball_box.position, new_ball_box.position, velocity);

			// alter rebound angle according to the position the ball touch the racket
			if (velocity.y > 0.0f)
			{
				float len   = glm::length(velocity);
				glm::vec2 v = velocity / len;
				float angle = std::atan2(v.y, v.x);

				float delta_position = (pawn_box.position.x - new_ball_box.position.x);
				float bound_factor = delta_position / pawn_box.half_size.x;
				bound_factor = std::clamp(bound_factor, -1.0f, 1.0f);

				float const TO_RAD = (float)M_PI / 180.0f;

				float rebound_angle_decrease = game->rebound_angle_decrease * TO_RAD;
				float rebound_angle_increase = game->rebound_angle_increase * TO_RAD;

				float const PI_2 = (float)M_PI_2;


				float const GOOD_LIMIT = 0.3f;
				float const BAD_LIMIT  = 0.6f;

				if (bound_factor < GOOD_LIMIT && bound_factor > -GOOD_LIMIT)
				{
					bound_factor = std::abs(bound_factor);
					if (angle > PI_2)
						angle = angle - rebound_angle_decrease * (1.0f - bound_factor / GOOD_LIMIT);
					else
						angle = angle + rebound_angle_decrease * (1.0f - bound_factor / GOOD_LIMIT);
				}
				else if (bound_factor > BAD_LIMIT)
				{
					bound_factor = std::abs(bound_factor);
					if (angle > PI_2)
						angle = angle + rebound_angle_increase * (bound_factor - BAD_LIMIT) / (1.0f - BAD_LIMIT);
					else
						angle = PI_2 - (angle - PI_2);
				}

				else if (bound_factor < -BAD_LIMIT)
				{
					bound_factor = std::abs(bound_factor);
					if (angle > PI_2)
						angle = PI_2 - (angle - PI_2);
					else
						angle = angle - rebound_angle_increase * (bound_factor - BAD_LIMIT) / (1.0f - BAD_LIMIT);
				}
				velocity = MakeVelocityFromAngle(angle);
			}

			ball_box.position = new_ball_box.position;
			game_instance->OnBallCollide(false);
		}
	}

	LudumLevelInstance * level_instance = game->GetLevelInstance();

	// bounce against bricks
	ParticleBrick * bricks = level_instance->GetBricks();
	size_t brick_count     = level_instance->GetBrickCount();

	if (bricks != nullptr && brick_count > 0)
	{
		for (size_t i = 0 ; i < brick_count ; ++i)
		{
			chaos::box2 brick_box = bricks[i].bounding_box;
			brick_box.position.y -= game_instance->brick_offset;

			if (chaos::RestrictToOutside(brick_box, new_ball_box))
			{
				if (bricks[i].indestructible || bricks[i].life >= game_instance->ball_power)
				{
					chaos::UpdateVelocityFromCollision(ball_box.position, new_ball_box.position, velocity);
					ball_box.position = new_ball_box.position;
				}

				if (!bricks[i].indestructible)
					bricks[i].life -= game_instance->ball_power;
				game_instance->OnBallCollide(true);

			}
		}
	}

	// recenter the particle
	particle.velocity = RestrictParticleVelocityToAngle(glm::normalize(velocity));
	particle.bounding_box = ball_box;

	return false;
}



void CompareDistanceAndReplace(float angle, float value, float & best_value, float & best_distance)
{
	float distance = std::abs(angle - value);
	if (distance < best_distance)
	{
		best_value = value;
		best_distance = distance;
	}
}

float ClampAngleToNearestRange(float angle, std::pair<float, float> const* ranges, size_t range_count)
{
	float const TO_RAD = (float)M_PI / 180.0f;

	float best_value = angle;
	float best_distance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < range_count; ++i)
	{
		float min_angle = ranges[i].first;
		float max_angle = ranges[i].second;

		// degree to rad

		min_angle = min_angle * TO_RAD;
		max_angle = max_angle * TO_RAD;
		if (min_angle > max_angle)
			std::swap(min_angle, max_angle);

		if (angle >= min_angle)
		{
			if (angle <= max_angle)
				return angle;
			CompareDistanceAndReplace(angle, max_angle, best_value, best_distance);
		}
		else
			CompareDistanceAndReplace(angle, min_angle, best_value, best_distance);
	}
	return best_value;
}

glm::vec2 ParticleMovableObjectLayerTrait::RestrictParticleVelocityToAngle(glm::vec2 const & v) const
{
	float ball_angle_min = game->ball_angle_min;
	float ball_angle_max = game->ball_angle_max;
	if (ball_angle_max <= 0.0f || ball_angle_min <= 0.0f)
		return v;

	float angle = atan2(v.y, v.x); // => [-PI , +PI]

	std::pair<float, float> ranges[] =
	{
		std::make_pair(180.0f - ball_angle_max,  90.0f + ball_angle_min),
		std::make_pair(90.0f - ball_angle_min, ball_angle_max),
		std::make_pair(-180.0f + ball_angle_max,  -90.0f - ball_angle_min),
		std::make_pair(-90.0f + ball_angle_min, -ball_angle_max)
	};

	angle = ClampAngleToNearestRange(angle, ranges, sizeof(ranges) / sizeof(ranges[0]));
	return MakeVelocityFromAngle(angle);
}

// ===========================================================================
// Challenge particle system
// ===========================================================================
void ParticleChallengeLayerTrait::ParticleToPrimitives(ParticleChallenge const& particle, chaos::GPUPrimitiveOutput<VertexBase>& output) const
{
	chaos::InputMode input_mode = particle.challenge->GetGameInstance()->GetPlayer(0)->GetInputMode();
    bool keyboard = chaos::IsPCMode(input_mode);

	ParticleChallenge other = particle;

	// tweak color
	if (keyboard)
	{
		size_t challenge_position = other.challenge->GetChallengePosition(false);
		if (other.index < challenge_position)
			other.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		else
			other.color = glm::vec4(1.0f, 0.41f, 0.0f, 1.0f);
	}
	else
	{
		size_t challenge_position = particle.challenge->GetChallengePosition(true);
		if (other.index < challenge_position)
			other.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		else
			other.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.50f);
	}
	chaos::ParticleToPrimitives(other, output);
}
