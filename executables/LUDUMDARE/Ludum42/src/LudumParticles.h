#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 

// ===========================================================================
// the ID of the layers
// ===========================================================================

static int const BACKGROUND_LAYER_ID             = 0;
static int const GAMEOBJECT_LAYER_ID             = 1;
static int const TEXT_LAYER_ID                   = 2;

// ===========================================================================
// Base Vertex class
// ===========================================================================

class VertexBase
{
public:

	glm::vec2 position;
	glm::vec3 texcoord;
	glm::vec4 color;
};

extern chaos::GPUVertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<VertexBase>);

// ===========================================================================
// Background particle system
// ===========================================================================

class ParticleBackground
{
public:

	glm::vec4 color;
};

class ParticleBackgroundTrait : public chaos::ParticleLayerTrait<ParticleBackground, VertexBase>
{
public:

	size_t ParticleToVertices(ParticleBackground const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;
};

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
// Some functions
// ===========================================================================

extern void DeclareParticleClasses();


