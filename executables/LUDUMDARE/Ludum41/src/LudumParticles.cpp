#pragma once

#include "LudumParticles.h"
#include "LudumSequenceChallenge.h"

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

bool ParticleBackgroundTrait::UpdateParticle(float delta_time, ParticleBackground * particle)
{

	return false;
}

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
// Challenge particle system
// ===========================================================================

bool ParticleChallengeTrait::UpdateParticle(float delta_time, ParticleChallenge * particle)
{

	return false;
}

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
