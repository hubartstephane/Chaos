#pragma once


#include "Ludum41Particles.h"
#include "Ludum41Game.h"
#include "Ludum41GameInstance.h"
#include "Ludum41LevelInstance.h"
#include "Ludum41Challenge.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>

// ===========================================================================
// Object particle system
// ===========================================================================

size_t ParticleObjectTrait::ParticleToVertices(ParticleObject const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
{
	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices);
	// copy the color in all triangles vertex
	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = particle->color;

	return vertices_per_particle;
}

bool ParticleObjectTrait::UpdateParticle(float delta_time, ParticleObject * particle, chaos::ParticleAllocation * allocation) const
{ 
	return false; 
}

// ===========================================================================
// Life particle system
// ===========================================================================

int ParticleLifeObjectTrait::BeginUpdateParticles(float delta_time, ParticleObject * particles, size_t count, chaos::ParticleAllocation * allocation, per_allocation_data & allocation_data) const
{
	allocation_data.rotation_time += delta_time;


	return count;
}

glm::vec2 ParticleLifeObjectTrait::BeginParticlesToVertices(ParticleObject const * particles, size_t count, chaos::ParticleAllocation * allocation, per_allocation_data const & allocation_data) const
{
	glm::vec2 result = glm::vec2(0.0f, 0.0f);


#if 1
	float S1 = 0.5f;
	float S2 = 1.0f;
	float BASE_R = 50.0f;

	float R = (2.0f + chaos::MathTools::Cos(S1 * allocation_data.rotation_time));

	result.x = BASE_R * R * chaos::MathTools::Cos(S2 * allocation_data.rotation_time);
	result.y = BASE_R * R * chaos::MathTools::Sin(S2 * allocation_data.rotation_time);
#endif
	return result;
}

bool ParticleLifeObjectTrait::UpdateParticle(float delta_time, ParticleObject * particle, chaos::ParticleAllocation * allocation, per_allocation_data & allocation_data, int extra_param) const
{
	return false;
}


size_t ParticleLifeObjectTrait::ParticleToVertices(ParticleObject const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation, per_allocation_data const & allocation_data, glm::vec2 const & extra_param) const
{
	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < 6; ++i)
		vertices[i].color = particle->color;

	return vertices_per_particle;
}

// ===========================================================================
// Brick particle system
// ===========================================================================

bool ParticleBrickTrait::UpdateParticle(float delta_time, ParticleBrick * particle, chaos::ParticleAllocation * allocation) const
{
	if (particle->life <= 0)
		return true;

	return false;
}

size_t ParticleBrickTrait::ParticleToVertices(ParticleBrick const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
{
	LudumGameInstance const * ludum_game_instance = game->GetLudumGameInstance();

	// generate particle corners and texcoords
	chaos::box2 bounding_box = particle->bounding_box;
	bounding_box.position.y -= ludum_game_instance->brick_offset;

	chaos::ParticleTools::GenerateBoxParticle(bounding_box, particle->texcoords, vertices);

	float extra = 2;
	float ratio = (extra + particle->life) / (extra + particle->starting_life);

	glm::vec4 color = ratio * particle->color;


	// copy the color in all triangles vertex
	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = color;

	return vertices_per_particle;
}


// ===========================================================================
// Object Movable particle system
// ===========================================================================

size_t ParticleMovableObjectTrait::ParticleToVertices(ParticleMovableObject const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
{
	LudumGameInstance const * ludum_game_instance = game->GetLudumGameInstance();

	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices);
	// copy the color in all triangles vertex

	glm::vec4 power_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	if (ludum_game_instance->ball_power == 0.5f)
		power_color = glm::vec4(0.0f, 0.58f, 1.0f, 1.0f);
	else if (ludum_game_instance->ball_power == 2.0f)
		power_color = glm::vec4(1.0f, 0.41f, 0.0f, 1.0f);
	else if (ludum_game_instance->ball_power == 3.0f)
		power_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = particle->color * power_color;

	return vertices_per_particle;
}


void ParticleMovableObjectTrait::UpdateParticleVelocityFromCollision(chaos::box2 const & ball_box, chaos::box2 const & new_ball_box, glm::vec2 & velocity) const
{
	if (ball_box.position.x > new_ball_box.position.x)
		velocity.x = -abs(velocity.x);				
	else if (ball_box.position.x < new_ball_box.position.x)
		velocity.x = abs(velocity.x);

	if (ball_box.position.y > new_ball_box.position.y)
		velocity.y = -abs(velocity.y);				
	else if (ball_box.position.y < new_ball_box.position.y)
		velocity.y = abs(velocity.y);

}

bool ParticleMovableObjectTrait::UpdateParticle(float delta_time, ParticleMovableObject * particle, chaos::ParticleAllocation * allocation) const
{
	LudumGameInstance * game_instance = game->GetLudumGameInstance();
	if (game_instance == nullptr)
		return false;

	// do not update particles during pause
	if (!game->IsPlaying())
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

	chaos::box2 view_box = game->GetViewBox();
	chaos::box2 ball_box = particle->bounding_box;
		

	// bounce against the world borders
	chaos::box2 new_ball_box = ball_box;
	if (chaos::RestrictToInside(view_box, new_ball_box, false))
	{
		glm::vec2 old_velocity = velocity;

		UpdateParticleVelocityFromCollision(ball_box, new_ball_box, velocity);
		ball_box.position = new_ball_box.position;

		if (old_velocity.y < 0.0f && velocity.y > 0.0f)
		{
		
			return true; // ball lost
		}
	}

	// bounce against player
	death::Player * player = game->GetPlayer(0);
	if (player == nullptr)
		return false;

	if (player != nullptr)
	{
		chaos::box2 player_box = player->GetPlayerBox();

		chaos::box2 new_ball_box = ball_box;
		if (chaos::RestrictToOutside(player_box, new_ball_box))
		{
			UpdateParticleVelocityFromCollision(ball_box, new_ball_box, velocity);
			ball_box.position = new_ball_box.position;
			game_instance->OnBallCollide(false);
		}
	}

	LudumLevelInstance * level_instance = game->GetLudumLevelInstance();

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
					UpdateParticleVelocityFromCollision(ball_box, new_ball_box, velocity);
					ball_box.position = new_ball_box.position;					
				}

				if (!bricks[i].indestructible)
					bricks[i].life -= game_instance->ball_power;
				game_instance->OnBallCollide(true);
			
			}				
		}	
	}

	// recenter the particle
	particle->velocity = RestrictParticleVelocityToAngle(glm::normalize(velocity));
	particle->bounding_box = ball_box;

	return false; 
}

glm::vec2 MakeVelocityFromAngle(float angle)
{
	return glm::vec2(cosf(angle), sinf(angle));
}

glm::vec2 ParticleMovableObjectTrait::RestrictParticleVelocityToAngle(glm::vec2 const & v) const
{
	float ball_angle_limit = game->ball_angle_limit;
	if (ball_angle_limit <= 0.0f)
		return v;

	float angle = atan2(v.y, v.x);

	if (angle > (float)M_PI - ball_angle_limit)
		return MakeVelocityFromAngle((float)M_PI - ball_angle_limit);

	if (angle < -(float)M_PI + ball_angle_limit)
		return MakeVelocityFromAngle(-(float)M_PI + ball_angle_limit);

	if (angle >= 0.0f && angle < ball_angle_limit)
		return MakeVelocityFromAngle(ball_angle_limit);	

	if (angle < 0.0f && angle >= -ball_angle_limit)
		return MakeVelocityFromAngle(-ball_angle_limit);

	return v;
}

// ===========================================================================
// Challenge particle system
// ===========================================================================


size_t ParticleChallengeTrait::ParticleToVertices(ParticleChallenge const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
{
	int  input_mode = chaos::MyGLFW::SingleWindowApplication::GetApplicationInputMode();
	bool keyboard = chaos::InputMode::IsPCMode(input_mode);


	size_t challenge_position = particle->challenge->GetChallengePosition();

	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices);

	// copy the color in all triangles vertex

	glm::vec4 color = particle->color;

	if (keyboard)
	{
		if (particle->index < challenge_position)
			color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		else
			color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		if (particle->index < challenge_position)
			color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		else
			color = glm::vec4(1.0f, 1.0f, 1.0f, 0.50f);
	}



	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = color;

	return vertices_per_particle;
}
