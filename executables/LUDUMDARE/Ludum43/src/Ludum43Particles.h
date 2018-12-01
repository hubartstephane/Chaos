#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 

// ===========================================================================
// VertexBase and Particle
// ===========================================================================

using VertexBase = chaos::ParticleDefault::Vertex;

using ParticleObject = chaos::ParticleDefault::Particle;

// ===========================================================================
// Object particle system
// ===========================================================================

using ParticleObject = chaos::ParticleDefault::Particle;

class ParticleObjectTrait : public chaos::ParticleLayerTrait<ParticleObject, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleObject * particle, chaos::ParticleAllocation * allocation) const;

	size_t ParticleToVertices(ParticleObject const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;
};

// ===========================================================================
// ParticleObjectAtlas
// ===========================================================================

class ParticleObjectAtlas : public ParticleObject
{
public:

	glm::ivec2 atlas_dimension = glm::vec2(1, 1);
	float frequency   = 0.0f; 
	int   skip_last   = 0;
	int   delta_image = 0;
};


class ParticleObjectAtlasTrait : public chaos::ParticleLayerTrait<ParticleObjectAtlas, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleObjectAtlas * particle, chaos::ParticleAllocation * allocation) const;

	size_t ParticleToVertices(ParticleObjectAtlas const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;

public:

	class LudumGame * game = nullptr;
};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleObjectAtlas
{

};

class ParticlePlayerTrait : public chaos::ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticlePlayer * particle, chaos::ParticleAllocation * allocation) const;

	size_t ParticleToVertices(ParticlePlayer const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;

public:

	class LudumGame * game = nullptr;
};
