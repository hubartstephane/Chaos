#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 

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

	void ParticleToVertex(ParticleChallenge const * particle, VertexChallenge * vertices) const
	{
#if 0
		chaos::ParticleCorners corners = chaos::ParticleTools::GetParticleCorners(particle->position, particle->size, chaos::Hotpoint::CENTER);

		chaos::ParticleTools::GenerateBoxParticle(corners, particle->texcoords, vertices);

		float alpha = particle->remaining_time / particle->lifetime;
		for (size_t i = 0 ; i < 6 ; ++i)
			vertices[i].color = glm::vec4(1.0f, 0.5f, 0.25f, alpha);
#endif
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

using ParticleLayerDescChallenge = chaos::TParticleLayerDesc<ParticleChallengeTrait>;



