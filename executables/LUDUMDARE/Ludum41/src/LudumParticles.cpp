#pragma once


#include "LudumParticles.h"
#include "LudumGame.h"
#include "LudumChallenge.h"

#include <chaos/CollisionFramework.h>

chaos::VertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<VertexBase>)
{
	chaos::VertexDeclaration result;
	result.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT2);
	result.Push(chaos::SEMANTIC_TEXCOORD, 0, chaos::TYPE_FLOAT3);
	result.Push(chaos::SEMANTIC_COLOR,    0, chaos::TYPE_FLOAT4);
	return result;
}

// ===========================================================================
// Background particle system
// ===========================================================================

size_t ParticleBackgroundTrait::ParticleToVertex(ParticleBackground const * particle, VertexBase * vertices, size_t vertices_per_particle) const
{
	vertices[0].position.x = -1.0;
	vertices[0].position.y = -1.0;

	vertices[1].position.x =  1.0;
	vertices[1].position.y = -1.0;

	vertices[2].position.x = -1.0;
	vertices[2].position.y =  1.0;

	vertices[3] = vertices[2];
	vertices[4] = vertices[1];

	vertices[5].position.x =  1.0;
	vertices[5].position.y =  1.0;

	// copy the vertices in all triangles vertex
	for (size_t i = 0 ; i < 6 ; ++i)
	{
		glm::vec2 texcoord = vertices[i].position * 0.5f + glm::vec2(0.5, 0.5);

		vertices[i].texcoord.x = texcoord.x;
		vertices[i].texcoord.y = texcoord.y;
		vertices[i].texcoord.z = 0.0f;
		vertices[i].color = particle->color;
	}

	return vertices_per_particle;
}

// ===========================================================================
// Object particle system
// ===========================================================================

size_t ParticleObjectTrait::ParticleToVertex(ParticleObject const * particle, VertexBase * vertices, size_t vertices_per_particle) const
{
	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->box, particle->texcoords, vertices);
	// copy the color in all triangles vertex
	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = particle->color;

	return vertices_per_particle;
}

bool ParticleObjectTrait::UpdateParticle(chaos::TypedUpdateParticleData<ParticleObject> & data)
{ 
	return false; 
}

// ===========================================================================
// Life particle system
// ===========================================================================


size_t ParticleLifeObjectTrait::ParticleToVertex(ParticleObject const * particle, VertexBase * vertices, size_t vertices_per_particle) const
{
	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->box, particle->texcoords, vertices);
	
	// create pulsating effect
	glm::vec4 color = particle->color;
	if (game->heart_warning < 0.5f)
		color.a = 0.4f + 0.6f * game->heart_warning / 0.5f;
	
	// copy the color in all triangles vertex
	for (size_t i = 0; i < 6; ++i)
		vertices[i].color = color;

	return vertices_per_particle;
}

// ===========================================================================
// Brick particle system
// ===========================================================================

bool ParticleBrickTrait::UpdateParticle(chaos::TypedUpdateParticleData<ParticleBrick> & data)
{
	if (data.particle->life <= 0)
		return true;

	return false;
}

size_t ParticleBrickTrait::ParticleToVertex(ParticleBrick const * particle, VertexBase * vertices, size_t vertices_per_particle) const
{
	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->box, particle->texcoords, vertices);

	int   extra = 2;
	float ratio = ((float)(extra + particle->life)) / ((float)(extra + particle->starting_life));

	glm::vec4 color = ratio * particle->color;


	// copy the color in all triangles vertex
	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = color;

	return vertices_per_particle;
}


// ===========================================================================
// Object Movable particle system
// ===========================================================================

size_t ParticleMovableObjectTrait::ParticleToVertex(ParticleMovableObject const * particle, VertexBase * vertices, size_t vertices_per_particle) const
{
	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->box, particle->texcoords, vertices);
	// copy the color in all triangles vertex
	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = particle->color;

	return vertices_per_particle;
}


void ParticleMovableObjectTrait::UpdateParticleVelocityFromCollision(chaos::box2 const & ball_box, chaos::box2 const & new_ball_box, glm::vec2 & velocity)
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

bool ParticleMovableObjectTrait::UpdateParticle(chaos::TypedUpdateParticleData<ParticleMovableObject> & data)
{
	// do not update particles during pause
	if (!game->IsPlaying())
		return false;

	ParticleMovableObject * particle = data.particle;
	float delta_time = data.delta_time;

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
	particle->box.position += velocity * 
		(game->ball_collision_speed + game->ball_speed) * 
		(delta_time * game->ball_time_dilation);

	// ball bouncing against world

	chaos::box2 world_box = game->GetWorldBox();
	chaos::box2 ball_box  = particle->box;
		

	// bounce against the world borders
	chaos::box2 new_ball_box = ball_box;
	if (chaos::RestrictToInside(world_box, new_ball_box, false))
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
	ParticleObject * player = game->GetPlayerParticle();
	if (player != nullptr)
	{
		chaos::box2 player_box = player->box;

		chaos::box2 new_ball_box = ball_box;
		if (chaos::RestrictToOutside(player_box, new_ball_box))
		{
			UpdateParticleVelocityFromCollision(ball_box, new_ball_box, velocity);
			ball_box.position = new_ball_box.position;
			game->OnBallCollide(false);			
		}
	}

	// bounce against bricks
	ParticleBrick * bricks = game->GetBricks();
	size_t brick_count     = game->GetBrickCount();

	if (bricks != nullptr && brick_count > 0)
	{
		for (size_t i = 0 ; i < brick_count ; ++i)
		{
			chaos::box2 brick_box = bricks[i].box;
		
			if (chaos::RestrictToOutside(brick_box, new_ball_box))
			{
				UpdateParticleVelocityFromCollision(ball_box, new_ball_box, velocity);
				ball_box.position = new_ball_box.position;

				if (bricks[i].destructible)
					--bricks[i].life;
				game->OnBallCollide(true);				
			}				
		}	
	}

	// recenter the particle
	particle->velocity = RestrictParticleVelocityToAngle(glm::normalize(velocity));
	particle->box = ball_box;

	return false; 
}

glm::vec2 MakeVelocityFromAngle(float angle)
{
	return glm::vec2(cosf(angle), sinf(angle));
}

glm::vec2 ParticleMovableObjectTrait::RestrictParticleVelocityToAngle(glm::vec2 const & v)
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


size_t ParticleChallengeTrait::ParticleToVertex(ParticleChallenge const * particle, VertexBase * vertices, size_t vertices_per_particle) const
{
	int  input_mode = chaos::MyGLFW::SingleWindowApplication::GetApplicationInputMode();
	bool keyboard = chaos::InputMode::IsPCMode(input_mode);


	size_t challenge_position = particle->challenge->GetChallengePosition();

	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->box, particle->texcoords, vertices);

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
