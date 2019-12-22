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

size_t ParticleObjectTrait::ParticleToVertices(ParticleObject const * particle, VertexBase * vertices, size_t vertices_per_particle) const
{
 	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices);
	// copy the color in all triangles vertex
	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = particle->color;

	return vertices_per_particle;
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

glm::vec2 ParticleLifeObjectTrait::BeginParticlesToVertices(chaos::ParticleConstAccessor<ParticleObject> const & particle_accessor, LayerTrait const * layer_trait) const
{
	return glm::vec2(0.0f, 0.0f);
}

bool ParticleLifeObjectTrait::UpdateParticle(float delta_time, ParticleObject * particle, int extra_param, LayerTrait const * layer_trait) const
{
	return false;
}

size_t ParticleLifeObjectTrait::ParticleToVertices(ParticleObject const * particle, VertexBase * vertices, size_t vertices_per_particle, glm::vec2 const & extra_param, LayerTrait const * layer_trait) const
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

bool ParticleBrickTrait::UpdateParticle(float delta_time, ParticleBrick * particle, LayerTrait const * layer_trait) const
{
	if (particle->life <= 0)
		return true;

	return false;
}

size_t ParticleBrickTrait::ParticleToVertices(ParticleBrick const * particle, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const
{
	LudumGameInstance const * ludum_game_instance = layer_trait->game->GetLudumGameInstance();

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

size_t ParticleMovableObjectTrait::ParticleToVertices(ParticleMovableObject const * particle, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const
{
	LudumGameInstance const * ludum_game_instance = layer_trait->game->GetLudumGameInstance();

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

bool ParticleMovableObjectTrait::UpdateParticle(float delta_time, ParticleMovableObject * particle, LayerTrait const * layer_trait) const
{
	LudumGameInstance * game_instance = layer_trait->game->GetLudumGameInstance();
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

	chaos::box2 canvas_box = layer_trait->game->GetCanvasBox();
	chaos::box2 ball_box = particle->bounding_box;
		

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
			ball_box.position = new_ball_box.position;
			game_instance->OnBallCollide(false);
		}
	}

	LudumLevelInstance * level_instance = layer_trait->game->GetLudumLevelInstance();

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
	particle->velocity = RestrictParticleVelocityToAngle(glm::normalize(velocity), layer_trait);
	particle->bounding_box = ball_box;

	return false; 
}

glm::vec2 MakeVelocityFromAngle(float angle)
{
	return glm::vec2(std::cos(angle), std::sin(angle));
}

glm::vec2 ParticleMovableObjectTrait::RestrictParticleVelocityToAngle(glm::vec2 const & v, LayerTrait const * layer_trait) const
{
	float ball_angle_limit = layer_trait->game->ball_angle_limit;
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


size_t ParticleChallengeTrait::ParticleToVertices(ParticleChallenge const * particle, VertexBase * vertices, size_t vertices_per_particle) const
{
	int  input_mode = chaos::MyGLFW::SingleWindowApplication::GetApplicationInputMode();
	bool keyboard = chaos::InputMode::IsPCMode(input_mode);

	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices);

	// copy the color in all triangles vertex

	glm::vec4 color = particle->color;

	if (keyboard)
	{
		size_t challenge_position = particle->challenge->GetChallengePosition(false);
		if (particle->index < challenge_position)
			color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		else
			color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		size_t challenge_position = particle->challenge->GetChallengePosition(true);
		if (particle->index < challenge_position)
			color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		else
			color = glm::vec4(1.0f, 1.0f, 1.0f, 0.50f);
	}

	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = color;

	return vertices_per_particle;
}
