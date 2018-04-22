#pragma once


#include "LudumParticles.h"
#include "LudumGame.h"
#include "LudumSequenceChallenge.h"

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
	chaos::ParticleTools::GenerateBoxParticle(particle->corners, particle->texcoords, vertices);
	// copy the color in all triangles vertex
	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = particle->color;

	return vertices_per_particle;
}

bool ParticleObjectTrait::UpdateParticle(float delta_time, ParticleObject * particle)
{ 
	return false; 
}


// ===========================================================================
// Object Movable particle system
// ===========================================================================

size_t ParticleMovableObjectTrait::ParticleToVertex(ParticleMovableObject const * particle, VertexBase * vertices, size_t vertices_per_particle) const
{
	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->corners, particle->texcoords, vertices);
	// copy the color in all triangles vertex
	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = particle->color;

	return vertices_per_particle;
}

bool ParticleMovableObjectTrait::UpdateParticle(float delta_time, ParticleMovableObject * particle)
{
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
	particle->velocity = glm::normalize(particle->velocity) * game->ball_speed;

	// moving the particle
	particle->corners.bottomleft += particle->velocity * delta_time * game->ball_time_dilation;
	particle->corners.topright   += particle->velocity * delta_time * game->ball_time_dilation;

	glm::vec2 position = (particle->corners.bottomleft + particle->corners.topright) * 0.5f;

	glm::vec2 particle_half_size = chaos::ParticleCornersToBox(particle->corners).half_size;

	// ball bouncing
	chaos::box2 world = game->GetWorldBox();

	std::pair<glm::vec2, glm::vec2> world_corners = world.GetCorners();

	
	// bounce against the world borders
	glm::vec2 new_position = position;

	if ((particle->corners.topright.x < world_corners.first.x) ||
		(particle->corners.bottomleft.x < world_corners.first.x && particle->velocity.x < 0.0f))
	{
		new_position.x = world_corners.first.x + particle_half_size.x;
		particle->velocity.x = -particle->velocity.x;	
	}
	else if ((particle->corners.bottomleft.x > world_corners.second.x) ||
		(particle->corners.topright.x > world_corners.second.x && particle->velocity.x > 0.0f))
	{
		new_position.x = world_corners.second.x - particle_half_size.x;
		particle->velocity.x = -particle->velocity.x;	
	}

	if ((particle->corners.topright.y < world_corners.first.y) ||
		(particle->corners.bottomleft.y < world_corners.first.y && particle->velocity.y < 0.0f))
	{
		new_position.y = world_corners.first.y + particle_half_size.y;
		particle->velocity.y = -particle->velocity.y;	
	}
	else if ((particle->corners.bottomleft.y > world_corners.second.y) ||
		(particle->corners.topright.y > world_corners.second.y && particle->velocity.y > 0.0f))
	{
		new_position.y = world_corners.second.y - particle_half_size.y;
		particle->velocity.y = -particle->velocity.y;	
	}
	// update particle
	particle->corners.bottomleft = new_position - particle_half_size;
	particle->corners.topright   = new_position + particle_half_size;

	// bounce against player

	ParticleObject * player = game->GetPlayerParticle();
	if (player != nullptr)
	{
		chaos::box2 player_box = chaos::ParticleCornersToBox(player->corners);
		chaos::box2 ball_box   = chaos::box2(new_position, particle_half_size);

		if (chaos::Collide(player_box, ball_box))
		{
			std::pair<glm::vec2, glm::vec2> player_corners = player_box.GetCorners();

			if (particle->corners.bottomleft.x < player_corners.second.x && particle->velocity.x < 0.0f)
			{
				new_position.x = player_corners.second.x + particle_half_size.x;
				particle->velocity.x = -particle->velocity.x;	
			}
			else if (particle->corners.topright.x > player_corners.first.x && particle->velocity.x > 0.0f)
			{
				new_position.x = player_corners.first.x - particle_half_size.x;
				particle->velocity.x = -particle->velocity.x;	
			}	



			if (particle->corners.bottomleft.y < player_corners.second.y && particle->velocity.y < 0.0f)
			{
				new_position.y = player_corners.second.y + particle_half_size.y;
				particle->velocity.y = -particle->velocity.y;	
			}
			else if (particle->corners.topright.y > player_corners.first.y && particle->velocity.y > 0.0f)
			{
				new_position.y = player_corners.first.y - particle_half_size.y;
				particle->velocity.y = -particle->velocity.y;	
			}
		}	
	}







	// recenter the particle
	particle->corners.bottomleft = new_position - particle_half_size;
	particle->corners.topright   = new_position + particle_half_size;

	return false; 
}


// ===========================================================================
// Challenge particle system
// ===========================================================================


size_t ParticleChallengeTrait::ParticleToVertex(ParticleChallenge const * particle, VertexBase * vertices, size_t vertices_per_particle) const
{
	size_t challenge_position = particle->challenge->GetChallengePosition();

	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->corners, particle->texcoords, vertices);

	// copy the color in all triangles vertex

	glm::vec4 color = particle->color;
	if (particle->index < challenge_position)
		color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	else
		color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);

	for (size_t i = 0 ; i < 6 ; ++i)
		vertices[i].color = color;

	return vertices_per_particle;
}
