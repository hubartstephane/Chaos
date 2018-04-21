#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 

// ===========================================================================
// the ID of the layers
// ===========================================================================

static int const BACKGROUND_LAYER_ID = 0;
static int const CHALLENGE_LAYER_ID  = 1;

// ===========================================================================
// Background particle system
// ===========================================================================

class ParticleBackground
{
public:

	glm::vec4 color;
};

class VertexBackground
{
public:

	glm::vec2 position;
	glm::vec2 texcoord;
	glm::vec4 color;
};

class ParticleBackgroundTrait : public chaos::ParticleLayerTrait<ParticleBackground, VertexBackground>
{
public:

	bool UpdateParticle(float delta_time, ParticleBackground * particle)
	{
		//	particle->position += particle->velocity * delta_time;
		//	particle->remaining_time -= delta_time;
		//	return (particle->remaining_time <= 0.0f);
		return false;
	}

	size_t ParticleToVertex(ParticleBackground const * particle, VertexBackground * vertices, size_t vertices_per_particle) const
	{
		//
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
			vertices[i].texcoord = vertices[i].position * 0.5f + glm::vec2(0.5, 0.5);			
			vertices[i].color = particle->color;
		}

		return vertices_per_particle;
	}

	chaos::VertexDeclaration GetVertexDeclaration() const
	{
		chaos::VertexDeclaration result;
		result.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT2);
		result.Push(chaos::SEMANTIC_TEXCOORD, 0, chaos::TYPE_FLOAT2);
		result.Push(chaos::SEMANTIC_COLOR,    0, chaos::TYPE_FLOAT4);
		return result;
	}
};


// ===========================================================================
// Challenge particle system
// ===========================================================================

class ParticleChallenge
{
public:

	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 size;
	chaos::ParticleTexcoords texcoords;
	float lifetime;
	float remaining_time;
};

class VertexChallenge
{
public:

	glm::vec2 position;
	glm::vec3 texcoord;
	glm::vec4 color;
};

class ParticleChallengeTrait : public chaos::ParticleLayerTrait<ParticleChallenge, VertexChallenge>
{
public:

	bool UpdateParticle(float delta_time, ParticleChallenge * particle)
	{
	//	particle->position += particle->velocity * delta_time;
	//	particle->remaining_time -= delta_time;
	//	return (particle->remaining_time <= 0.0f);
		return false;
	}

	size_t ParticleToVertex(ParticleChallenge const * particle, VertexChallenge * vertices, size_t vertices_per_particle) const
	{
#if 0
		chaos::ParticleCorners corners = chaos::ParticleTools::GetParticleCorners(particle->position, particle->size, chaos::Hotpoint::CENTER);

		chaos::ParticleTools::GenerateBoxParticle(corners, particle->texcoords, vertices);

		float alpha = particle->remaining_time / particle->lifetime;
		for (size_t i = 0 ; i < 6 ; ++i)
			vertices[i].color = glm::vec4(1.0f, 0.5f, 0.25f, alpha);
#endif
		return vertices_per_particle;
	}

	chaos::VertexDeclaration GetVertexDeclaration() const
	{
		chaos::VertexDeclaration result;
		result.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT2);
		result.Push(chaos::SEMANTIC_TEXCOORD, 0, chaos::TYPE_FLOAT3);
		result.Push(chaos::SEMANTIC_COLOR,    0, chaos::TYPE_FLOAT4);
		return result;
	}
};




