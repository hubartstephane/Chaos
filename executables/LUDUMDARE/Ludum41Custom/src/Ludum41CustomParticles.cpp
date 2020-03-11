#pragma once


#include "Ludum41CustomParticles.h"
#include "Ludum41CustomGame.h"
#include "Ludum41CustomGameInstance.h"
#include "Ludum41CustomLevelInstance.h"
#include "Ludum41CustomChallenge.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>
#include <chaos/PrimitiveOutput.h>

// ===========================================================================
// Object particle system
// ===========================================================================

void ParticleObjectTrait::ParticleToPrimitives(ParticleObject const& particle, chaos::QuadOutput<VertexBase>& output) const
{
    chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();

    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);
    // copy the color in all triangles vertex
    for (size_t i = 0; i < primitive.count; ++i)
        primitive[i].color = particle.color;
}

void ParticleObjectTrait::ParticleToPrimitives(ParticleObject const& particle, chaos::TrianglePairOutput<VertexBase>& output) const
{
    chaos::TrianglePairPrimitive<VertexBase> primitive = output.AddPrimitive();

    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);
    // copy the color in all triangles vertex
    for (size_t i = 0; i < primitive.count; ++i)
        primitive[i].color = particle.color;
}

bool ParticleObjectTrait::UpdateParticle(float delta_time, ParticleObject * particle) const
{ 
	return false; 
}

// ===========================================================================
// Life particle system
// ===========================================================================

int ParticleLifeObjectTrait::BeginUpdateParticles(float delta_time, chaos::ParticleAccessor<ParticleObject>& particle_accessor, LayerTrait const * layer_trait) const
{
	return 0;
}

glm::vec2 ParticleLifeObjectTrait::BeginParticlesToPrimitives(chaos::ParticleConstAccessor<ParticleObject> const & particle_accessor, LayerTrait const * layer_trait) const
{
	return glm::vec2(0.0f, 0.0f);
}

bool ParticleLifeObjectTrait::UpdateParticle(float delta_time, ParticleObject * particle, int extra_param, LayerTrait const * layer_trait) const
{
	return false;
}

void ParticleLifeObjectTrait::ParticleToPrimitives(ParticleObject const& particle, chaos::QuadOutput<VertexBase>& output, glm::vec2 const& extra_param, LayerTrait const* layer_trait) const
{
    chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();

    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);
    // copy the color in all triangles vertex
    for (size_t i = 0; i < primitive.count; ++i)
        primitive[i].color = particle.color;
}

void ParticleLifeObjectTrait::ParticleToPrimitives(ParticleObject const& particle, chaos::TrianglePairOutput<VertexBase>& output, glm::vec2 const& extra_param, LayerTrait const* layer_trait) const
{
    chaos::TrianglePairPrimitive<VertexBase> primitive = output.AddPrimitive();

    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);
    // copy the color in all triangles vertex
    for (size_t i = 0; i < primitive.count; ++i)
        primitive[i].color = particle.color;
}





// ===========================================================================
// Brick particle system
// ===========================================================================

bool ParticleBrickTrait::UpdateParticle(float delta_time, ParticleBrick * particle, LayerTrait const * layer_trait) const
{
	if (particle->life <= 0)
		return true;
	particle->highlight_time = std::max(0.0f, particle->highlight_time - delta_time);
	return false;
}

void ParticleBrickTrait::ParticleToPrimitives(ParticleBrick const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    LudumGameInstance const* ludum_game_instance = layer_trait->game->GetGameInstance();

    chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();

    // generate particle corners and texcoords
    chaos::box2 bounding_box = particle.bounding_box;
    bounding_box.position.y -= ludum_game_instance->brick_offset;
    chaos::ParticleTools::GenerateBoxParticle(bounding_box, particle.texcoords, primitive);

    // copy the color in all triangles vertex
    float extra = 2;
    float ratio = (extra + particle.life) / (extra + particle.starting_life);
    glm::vec4 color = ratio * particle.color;

	if (particle.highlight_time > 0.0f)
		color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);    
    for (size_t i = 0; i < primitive.count; ++i)
        primitive[i].color = color;
}

void ParticleBrickTrait::ParticleToPrimitives(ParticleBrick const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    LudumGameInstance const* ludum_game_instance = layer_trait->game->GetGameInstance();

    chaos::TrianglePairPrimitive<VertexBase> primitive = output.AddPrimitive();

    // generate particle corners and texcoords
    chaos::box2 bounding_box = particle.bounding_box;
    bounding_box.position.y -= ludum_game_instance->brick_offset;
    chaos::ParticleTools::GenerateBoxParticle(bounding_box, particle.texcoords, primitive);

    // copy the color in all triangles vertex
    float extra = 2;
    float ratio = (extra + particle.life) / (extra + particle.starting_life);
    glm::vec4 color = ratio * particle.color;

    for (size_t i = 0; i < primitive.count; ++i)
        primitive[i].color = color;
}



// ===========================================================================
// Object Movable particle system
// ===========================================================================

void ParticleMovableObjectTrait::ParticleToPrimitives(ParticleMovableObject const& particle, chaos::QuadOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    LudumGameInstance const* ludum_game_instance = layer_trait->game->GetGameInstance();

    chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();
    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);
    // copy the color in all triangles vertex

    glm::vec4 power_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    if (ludum_game_instance->ball_power == 0.5f)
        power_color = glm::vec4(0.0f, 0.58f, 1.0f, 1.0f);
    else if (ludum_game_instance->ball_power == 2.0f)
        power_color = glm::vec4(1.0f, 0.41f, 0.0f, 1.0f);
    else if (ludum_game_instance->ball_power == 3.0f)
        power_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    for (size_t i = 0; i < primitive.count; ++i)
        primitive[i].color = particle.color * power_color;
}

void ParticleMovableObjectTrait::ParticleToPrimitives(ParticleMovableObject const& particle, chaos::TrianglePairOutput<VertexBase>& output, LayerTrait const* layer_trait) const
{
    LudumGameInstance const* ludum_game_instance = layer_trait->game->GetGameInstance();

    chaos::TrianglePairPrimitive<VertexBase> primitive = output.AddPrimitive();
    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);
    // copy the color in all triangles vertex

    glm::vec4 power_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    if (ludum_game_instance->ball_power == 0.5f)
        power_color = glm::vec4(0.0f, 0.58f, 1.0f, 1.0f);
    else if (ludum_game_instance->ball_power == 2.0f)
        power_color = glm::vec4(1.0f, 0.41f, 0.0f, 1.0f);
    else if (ludum_game_instance->ball_power == 3.0f)
        power_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    for (size_t i = 0; i < primitive.count; ++i)
        primitive[i].color = particle.color * power_color;
}









void ParticleMovableObjectTrait::UpdateParticleVelocityFromCollision(glm::vec2 const & old_position, glm::vec2 const & new_position, glm::vec2 & velocity) const
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

bool ParticleMovableObjectTrait::UpdateParticle(float delta_time, ParticleMovableObject * particle, LayerTrait const * layer_trait) const
{
	LudumGameInstance * game_instance = layer_trait->game->GetGameInstance();
	if (game_instance == nullptr)
		return false;

	// do not update particles during pause
	if (!layer_trait->game->IsPlaying() || layer_trait->game->IsFreeCameraMode())
		return false;

	// delay before moving the particle
	if (particle->delay_before_move > 0.0f)
	{
		particle->delay_before_move -=delta_time;
		if (particle->delay_before_move > 0.0f)
			return false;
	}

	// update the velocity of the ball
	glm::vec2 velocity = glm::normalize(particle->velocity);
	
	// moving the particle
	particle->bounding_box.position += velocity *
		(game_instance->ball_collision_speed + game_instance->ball_speed) *
		(delta_time * game_instance->ball_time_dilation);

	// ball bouncing against world

	chaos::box2 level_box = layer_trait->game->GetLevelInstance()->GetBoundingBox();
	chaos::box2 ball_box = particle->bounding_box;
		

	// bounce against the world borders
	chaos::box2 new_ball_box = ball_box;
	if (chaos::RestrictToInside(level_box, new_ball_box, false))
	{
		glm::vec2 old_velocity = velocity;

		chaos::UpdateVelocityFromCollision(ball_box.position, new_ball_box.position, velocity);
		ball_box.position = new_ball_box.position;

		if (old_velocity.y < 0.0f && velocity.y > 0.0f)
		{
		
			return true; // ball lost
		}

		game_instance->OnBallCollide(false);
	}

	// bounce against player
	death::Player * player = layer_trait->game->GetPlayer(0);
	if (player == nullptr)
		return false;

	if (player != nullptr)
	{
		chaos::box2 player_box = player->GetPlayerBox();

		chaos::box2 new_ball_box = ball_box;
		if (chaos::RestrictToOutside(player_box, new_ball_box))
		{
			chaos::UpdateVelocityFromCollision(ball_box.position, new_ball_box.position, velocity);

			// alter rebound angle according to the position the ball touch the racket
			if (velocity.y > 0.0f)
			{
				float len   = glm::length(velocity);
				glm::vec2 v = velocity / len;
				float angle = std::atan2(v.y, v.x);

				float delta_position = (player_box.position.x - new_ball_box.position.x);
				float bound_factor = delta_position / player_box.half_size.x;
				bound_factor = std::clamp(bound_factor, -1.0f, 1.0f);

				float const TO_RAD = (float)M_PI / 180.0f;

				float rebound_angle_decrease = layer_trait->game->rebound_angle_decrease * TO_RAD;
				float rebound_angle_increase = layer_trait->game->rebound_angle_increase * TO_RAD;

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

	LudumLevelInstance * level_instance = layer_trait->game->GetLevelInstance();

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
				
				if (bricks[i].starting_life > 1.0f || bricks[i].indestructible)
					bricks[i].highlight_time = 0.05f;
				game_instance->OnBallCollide(true);


			
			}				
		}	
	}

	// recenter the particle
	particle->velocity = RestrictParticleVelocityToAngle(glm::normalize(velocity), layer_trait);
	particle->bounding_box = ball_box;

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

glm::vec2 ParticleMovableObjectTrait::RestrictParticleVelocityToAngle(glm::vec2 const & v, LayerTrait const * layer_trait) const
{
	float ball_angle_min = layer_trait->game->ball_angle_min;
	float ball_angle_max = layer_trait->game->ball_angle_max;
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
void ParticleChallengeTrait::ParticleToPrimitives(ParticleChallenge const& particle, chaos::QuadOutput<VertexBase>& output) const
{
	chaos::InputMode input_mode = particle.challenge->GetGameInstance()->GetPlayer(0)->GetInputMode();
    bool keyboard = chaos::IsPCMode(input_mode);

    chaos::QuadPrimitive<VertexBase> primitive = output.AddPrimitive();

    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);

    // copy the color in all triangles vertex

    glm::vec4 color = particle.color;

    if (keyboard)
    {
        size_t challenge_position = particle.challenge->GetChallengePosition(false);
        if (particle.index < challenge_position)
            color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        else
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        size_t challenge_position = particle.challenge->GetChallengePosition(true);
        if (particle.index < challenge_position)
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        else
            color = glm::vec4(1.0f, 1.0f, 1.0f, 0.50f);
    }

    for (size_t i = 0; i < primitive.count ; ++i)
        primitive[i].color = color;
}


void ParticleChallengeTrait::ParticleToPrimitives(ParticleChallenge const& particle, chaos::TrianglePairOutput<VertexBase>& output) const
{
	chaos::InputMode input_mode = particle.challenge->GetGameInstance()->GetPlayer(0)->GetInputMode();
    bool keyboard = chaos::IsPCMode(input_mode);
	   	  
    chaos::TrianglePairPrimitive<VertexBase> primitive = output.AddPrimitive();

    // generate particle corners and texcoords
    chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, primitive);

    // copy the color in all triangles vertex

    glm::vec4 color = particle.color;

    if (keyboard)
    {
        size_t challenge_position = particle.challenge->GetChallengePosition(false);
        if (particle.index < challenge_position)
            color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        else
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        size_t challenge_position = particle.challenge->GetChallengePosition(true);
        if (particle.index < challenge_position)
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        else
            color = glm::vec4(1.0f, 1.0f, 1.0f, 0.50f);
    }

    for (size_t i = 0; i < primitive.count; ++i)
        primitive[i].color = color;
}


